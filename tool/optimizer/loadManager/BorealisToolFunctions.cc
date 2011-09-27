#include "BorealisToolFunctions.h"
#include "LMNodeManager.h"
#include "LMStatisticsManager.h"


BOREALIS_NAMESPACE_BEGIN

void BorealisToolFunctions::write_plan(const string& filename)
{
    string prefix = LMParams::runtime_param.output_dir + "xml/" +
                 LMParams::runtime_param.output_prefix + filename;

    string deploy_filename = prefix + "_deploy.xml";
    string io_filename = prefix + "_io.xml";


    fstream deploy_fs;
    fstream io_fs;
    deploy_fs.open(deploy_filename.c_str(), ios::out);
    io_fs.open(io_filename.c_str(), ios::out);
    if (!deploy_fs)
    {
        WARN << "can not open file " << deploy_filename;
        return;
    }
    if (!io_fs)
    {
        WARN << "can not open file " << io_filename;
        return;
    }

    deploy_fs << "<?xml version=\"1.0\"?>\n";
    deploy_fs << "<deploy>\n";

    io_fs << "<?xml version=\"1.0\"?>\n";
    io_fs << "<deploy>\n";

    const LMArcMap& input_arcs = LMQueryNetwork::get_singleton()->get_input_arcs();
    for (LMArcMap::const_iterator it = input_arcs.begin();
            it != input_arcs.end();
            ++it)
    {
        const IdType& arc_id = it->first;
        LMArc* arc = it->second;
        string stream_name = get_stream_id(arc_id);

        const IdType& dest_box_id = arc->get_destination_box()->get_id();
        const string& node_name =
            LMNodeManager::get_singleton()->get_box_host_lmnode_id(dest_box_id);

        deploy_fs << "    <publish stream=\"" << stream_name << "\""
                            << "    " << "node=\"" << node_name << "\" />\n";
        io_fs << "    <publish stream=\"" << stream_name << "\""
                    << "    " << "node=\"" << node_name << "\" />\n";
    }



    deploy_fs << "\n";
    io_fs << "\n";

    const LMArcMap& output_arcs = LMQueryNetwork::get_singleton()->get_output_arcs();
    for (LMArcMap::const_iterator it = output_arcs.begin();
            it != output_arcs.end();
            ++it)
    {
        const IdType& arc_id = it->first;
        LMArc* arc = it->second;

        string stream_name = get_stream_id(arc_id);
        string src_box_id = arc->get_source_box()->get_id();
        string node_name =
            LMNodeManager::get_singleton()->get_box_host_lmnode_id(src_box_id);

        deploy_fs << "    <subscribe stream=\"" << stream_name << "\""
                            << "    " << "endpoint=\"" << "127.0.0.1:60000" << "\" />\n";
        io_fs << "    <subscribe stream=\"" << stream_name << "\""
                    << "    " << "endpoint=\"" << node_name << "\" />\n";
    }

    deploy_fs << "\n";

    const LMNodeMap& nodes = LMNodeManager::get_singleton()->get_lmnodes();
    for (LMNodeMap::const_iterator it = nodes.begin();
            it != nodes.end();
            ++it)
    {
        const IdSet& boxes = it->second.get_boxes();
        if (boxes.empty())
            continue;

        IdType node_id = it->first;
        deploy_fs << "    <node endpoint=\"" << node_id << "\"     query=\"";
        bool first = true;
        for (IdSet::const_iterator bit = boxes.begin();
                bit != boxes.end();
                ++bit)
        {
            if (first)
                first = false;
            else
                deploy_fs << " ";
            deploy_fs << *bit;
        }
        deploy_fs << "\" />\n";
    }

    deploy_fs << "\n";

    for (LMNodeMap::const_iterator it = nodes.begin();
            it != nodes.end();
            ++it)
    {
        deploy_fs << "    <region node=\"" << it->first << "\""
                            << "    endpoint=\"127.0.0.1:40000\" />\n";
    }

    deploy_fs << "</deploy>\n";
    io_fs << "</deploy>\n";


    deploy_fs.close();
    io_fs.close();

}



BOREALIS_NAMESPACE_END
