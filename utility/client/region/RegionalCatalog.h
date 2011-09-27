#ifndef  REGIONALCATALOG_H
#define  REGIONALCATALOG_H

#include  "Diagram.h"

BOREALIS_NAMESPACE_BEGIN


class  RegionalCatalog : public Diagram
{
  public:

    RegionalCatalog()
        : _loaded(False)
    {}


    /// Parse update Xml reflected to a region server from a Borealis node.
    ///
    Status  parse_update_xml( /// Update Xml to parse.
                            string  update_xml );


    /// Returns all control ports for all borealis nodes in a region.
    /// Use these to establish a MedusaClient to make RPC calls to the node.
    ///
    set<string>  *get_region_node()
    {   return( &_node );
    }


    ///  Check if the Regional Catalog is ready to use.
    ///
    Boolean  is_region_loaded() { return _loaded; }


    /// Returns the names of all nodes in a string
    string  as_string();


    ///  Wrapper for a request to insert a box on a stream.
    ///
    Status  connect_box(/// XML for a box with one in and one out stream.
                        string   box_xml,

                        /// Box to receive the output.
                        Name     target_box,

                        /// In port on the target box.
                        uint16   target_port = 0,

                        /// True to always deploy the box on the downstream node.
                        Boolean  downstream = False);


    ///  Wrapper for a request to remove a box and the stream is is on.
    ///
    Status  disconnect_stream(/// Added stream to disconnect.
                              Name     stream,

                              /// Original stream if it is upstream.
                              /// Empty if a local connect.
                              Name     upstream = "");


  private:

    ///  Add an input stream or new subscription to a regional catalog.
    ///
    void  add_input_element(const DOMElement  *input)
                      throw(AuroraBadEntityException);


    ///  Add a single parsed region update into a regional catalog structure.
    ///
    void  add_region_element(const DOMElement  *Region)
                       throw(AuroraBadEntityException);


    ///  Delete a single parsed element from a regional catalog structure.
    ///
    void  delete_element(const DOMElement  *Delete)
                   throw(AuroraBadEntityException);


    ///  Move or remove a box or table from a regional catalog structure.
    ///
    void  move_element(const DOMElement  *move)
                 throw(AuroraException);


    /// Reconnect a box port to a different stream when adding a drop box.
    ///
    void  connect_element(const DOMElement  *connect)
                    throw(AuroraException);


    ///  The downstream node reflects a modify In on a connect and disconnect.
    ///  Box and table parameters may be modified.
    ///
    void modify_element(const DOMElement  *modify)
                  throw(AuroraBadEntityException);


    /// Reconnect a box port to it's original stream when removing a drop box.
    ///
    void  disconnect_element(const DOMElement  *disconnect)
                       throw(AuroraException);


    ///  Virtual override to relocate existing boxes and add new boxes.
    ///
    CatalogBox  *add_box_element(const DOMElement  *box,
                                          Boolean   dynamic = False)
                           throw(AuroraBadEntityException);


    /// Clear the entire catalog.
    ///
    void  clear_catalog();


    /// All borealis nodes in a region.  Empty if a Global catalog.
    /// Each name contains:  <dotted IP address>:<control port>
    ///
    set<string>    _node;


    /// True after a global catalog has finished loading;
    ///
    Boolean        _loaded;
};

BOREALIS_NAMESPACE_END
#endif                       // REGIONALCATALOG_H
