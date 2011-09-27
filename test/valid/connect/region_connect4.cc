////////////////////////////////////////////////////////////////////////////////
//
// This test covers the case where the region contains only the upstream node.
// It can not call the connect/disconnect wrappers because the destination
// box is not in the region.
//..............................................................................


#define  CONNECT4
#include "region_connect_head.cc"

    string         node;
    Remote<QueryProcessor> query_processor;
    ptr<MedusaClient> client;



////////////////////////////////////////////////////////////////////////////////
//
static void  send_xml(string  *xml)
{
    RPC<void>       rpc;
//..............................................................................


    DEBUG << "xml=" << *xml;

    rpc = query_processor.add_xml_string(*xml);
    DEBUG << "add_xml_string status=" << rpc;

    if (!rpc.stat())
    {   WARN << "add_xml_string failed.";
    }
 
    return;
}


////////////////////////////////////////////////////////////////////////////////
//
void  write_regional_catalog()
{
    Status        status = true;
    set<string>  *node_set;
    pthread_t     thread;
//..............................................................................


    WriteRegion::write_region(server->get_regional_catalog(),
                              "Regiontest.xml");

    NOTICE << "wrote the catalog.";
    node_set = server->get_regional_catalog().get_region_node();

    if ((call == 0)  &&  (node_set->size() == 1))
    {   call = 1;

        xml  = "<connect  box=\"box1\"  port=\"0\"  upstream=\"1\" >\n";
        xml += " <box  name=\"Insert\"  type=\"filter\" >\n";
        xml += "  <in       stream=\"source\"       />\n";
        xml += "  <out      stream=\"intermediate\" />\n";
        xml += "  <parameter  name=\"expression.0\"  value=\"1=1\" />\n";
        xml += " </box>\n";
        xml += "</connect>\n";

        DEBUG << "xml=" << xml;

        if (pthread_create(&thread, 0,
                           (void*(*)(void*))send_xml,
                           (void*)&xml)
            < 0)
        {
            WARN << "Can not start the add_xml thread.";
        }
    }
    else if (call == 1)
    {   call = 2;
        xml  = "<disconnect  stream=\"intermediate\"  upstream=\"source\" />\n";

        if (pthread_create(&thread, 0,
                           (void*(*)(void*))send_xml,
                           (void*)&xml)
            < 0)
        {
            WARN << "Can not start the add_xml thread.";
        }
    }
 
    // The callback is enqueued with a timer.
    // We only callback with a timer because this is looping.
    //
    ( new CallbackTimer( loop,
                        &write_regional_catalog ))
              ->arm( Time::now() + Time::msecs( SLEEP_TIME ));

    return;
}

#include "region_connect_main.cc"
