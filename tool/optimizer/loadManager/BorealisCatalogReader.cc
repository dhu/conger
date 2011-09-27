#include "BorealisCatalogReader.h"
#include "LMQueryNetwork.h"
#include "LMNodeManager.h"
#include "LMParams.h"
#include <WriteRegion.h>

BOREALIS_NAMESPACE_BEGIN


bool BorealisCatalogReader::read_catalog(RegionalCatalog& catalog)
{
    //DEBUG << catalog.as_string();

    // read borealis node address
    set<string>& node_addresses = *(catalog.get_region_node());
    if (node_addresses.empty())
    {
        if (LMParams::display_param.show_basic_info)
            NOTICE << "No node is running in the region.";
        return false;
    }

    // clear old informaton
    LMNodeManager::get_singleton()->clear();
    LMQueryNetwork::get_singleton()->clear();


    // create nodes
    for (set<string>::const_iterator it = node_addresses.begin();
            it != node_addresses.end();
            ++it)
    {
        LMNodeManager::get_singleton()->add_lmnode(*it);
    }

    // Create boxes and fill in box location
    Catalog::BoxMap* box_map = catalog.get_box_map();
    for (Catalog::BoxMap::iterator it = box_map->begin();
            it != box_map->end();
            ++it)
    {
        // Create LMBox using its name as ID
        IdType box_id = it->first.as_string();
        LMBox* box = new LMBox(box_id, it->second.get_box_type());

        // Add this box to the LMQueryNetwork
        LMQueryNetwork::get_singleton()->add_box(box);

        // Add this box to its LMNode
        string node_name = it->second.get_box_node();
        IdSet box_ids;
        box_ids.insert(box_id);
        LMNodeManager::get_singleton()->add_boxes(node_name, box_ids);
    }

    // Create arcs
    Catalog::StreamMap* stream_map = catalog.get_stream_map();
    for (Catalog::StreamMap::iterator it = stream_map->begin();
            it != stream_map->end();
            ++it)
    {
        CatalogStream& catalog_stream = it->second;
        IdType stream_name(it->first.as_string());

        // Get stream tuple size
        // TODO add this after catalog schema propogation is implemented 
        //unsigned int stream_tuple_size = catalog_stream.getSchema()->getSize();
        static unsigned int stream_tuple_size = 12;
        DEBUG << "Schema need to be propagated in catalog.";

        // Get input box and port information
        // Each stream only has one input box
        const BoxPort* input_box_port = catalog_stream.get_stream_source();
        LMBox* input_box = 0;  // defalt value 0 for input arc
        PortId input_port = 0;
        // set correct box and port value for internal boxes
        if (!catalog_stream.is_input_stream())
        {
            // get the input LMBox handle from LMQueryNetwork given box name
            input_box = LMQueryNetwork::get_singleton()->get_box(
                    input_box_port->get_port_box()->get_box_name().as_string());
            input_port = (PortId)input_box_port->get_port();
        }

        // Get input box ID. It is the same for all arcs related to this stream
        IdType input_box_id = input_box != 0 ? input_box->get_id() : " ";

        // Each stream may have multiple sinks (corresponds to steam split).
        // for each output box and port, create an LMArc
        const vector<BoxPort>* sink_box_ports = catalog_stream.get_sink_port();
        for (unsigned int i = 0; i < sink_box_ports->size(); ++i)
        {
            const BoxPort& output_box_port = (*sink_box_ports)[i];

            LMBox* output_box = 0; // defalt value 0 for ouput arc
            PortId output_port = 0;

            if (output_box_port.get_port_box() != NULL)
            {
                // get the output LMBox handle from LMQueryNetwork given box name
                output_box = LMQueryNetwork::get_singleton()->get_box(
                      output_box_port.get_port_box()->get_box_name().as_string());
                output_port = (PortId)(output_box_port.get_port());
            }

            // Get output Box ID. One per arc
            IdType output_box_id = output_box != 0 ? output_box->get_id() : " ";

            // generate arc ID using stream name and I/O box name
            IdType arc_id = get_arc_id(
                    stream_name,
                    input_box_id,
                    output_box_id);

            // create an LMArc
            LMArc* arc = new LMArc(
                    arc_id,
                    input_box,
                    input_port,
                    output_box,
                    output_port,
                    stream_tuple_size);

            // add arc to query network and update map entry
            LMQueryNetwork::get_singleton()->add_arc(arc);
            LMQueryNetwork::get_singleton()->add_stream_idarc_idmap_entry(
                    stream_name, arc_id);
        }

        // If the stream is a output stream (no output box), then just creat
        // one output arc
        if (sink_box_ports->empty())
        {
            LMBox* output_box = 0;
            PortId output_port = 0;

            // create arc ID using empty ouput box name
            IdType arc_id = get_arc_id(
                    stream_name,
                    input_box_id,
                    " ");

            // create an LMArc
            LMArc* arc = new LMArc(
                    arc_id,
                    input_box,
                    input_port,
                    output_box,
                    output_port,
                    stream_tuple_size);

            // add arc to query network and update map entry
            LMQueryNetwork::get_singleton()->add_arc(arc);
            LMQueryNetwork::get_singleton()->add_stream_idarc_idmap_entry(
                    stream_name, arc_id);
        }
    }

    return true;
}


BOREALIS_NAMESPACE_END
