#include "args.h"
#include "Dynamic_Mod_TestMarshal.h"

uint32 BATCH_SIZE = 10;

int iterations = 0;
int stable_condition = 30;

// Variables for testing add/remove/connect/disconnect
bool added = false;
bool use_connect = true;

void test_add_remove(MedusaClient* _client)
{
    Name box_name("pretty_likely");

    if (added)
    {
        RPC<void> r;
        r = _client->test_remove_box(box_name);

        if ( !r.stat() )
            ERROR << r.stat();

    } else
    {

        string box_type = "filter";
        Name query_name("compute_risk");
        string node = _client->as_string();

        vector< pair<string, string> > params;
        params.push_back(make_pair("expression.0", "probability < 0.66"));

        NOTICE << "Dynamically modifiying query...";


        string input_stream = "event";
        string output_stream = "dynamic";
        string stream_xml = "<stream name=\"" + output_stream + "\" schema=\"dmt_schema\"/>";

        AsyncRPC<void> r = _client->add_stream_xml(stream_xml);
        if (!r.stat())
            ERROR << r.stat();

        while (!r.avail())
            Thread::sleep(Time::msecs(10));

        RPC<void> r2 = _client->test_add_box(box_name, box_type, query_name, node,
                                             input_stream, output_stream, params);
        if (!r2.stat())
            ERROR << r2.stat();

        iterations = 0;
    }

    added = !added;
}

void test_connect_disconnect(MedusaClient* _client)
{
    Name box_name("pretty_likely");

    if (added)
    {
        RPC<void> r;
        r = _client->test_disconnect_box(box_name);

        if ( !r.stat() )
            ERROR << r.stat();

    } else
    {

        string box_type = "filter";
        Name query_name("compute_risk");
        string node = _client->as_string();

        vector< pair<string, string> > params;
        params.push_back(make_pair("expression.0", "probability < 0.66"));

        NOTICE << "Dynamically modifiying query...";

        string source_stream = "likely";
        Name dest_box_name("box_b");
        uint16 dest_box_port = 0;

        RPC<void> r = _client->test_connect_box(box_name, box_type, query_name, node,
                                                source_stream, dest_box_name, dest_box_port,
                                                params);
        if ( !r.stat() )
            ERROR << r.stat();

        iterations = 0;
    }

    added = !added;
}

void Dynamic_Mod_TestMarshal::receivedUnlikely(dmt_schema* tuple)
{
    NOTICE << "Event p = " << tuple->probability;
}

void Dynamic_Mod_TestMarshal::sentEvent()
{

    if (iterations > stable_condition)
    {

        if ( use_connect )
            test_connect_disconnect(_client);
        else
            test_add_remove(_client);

    }

    for (uint32 i = 0; i < BATCH_SIZE; i++)
    {
    Event tuple;
    tuple._data.probability = drand48();

    batchEvent( &tuple );
    }

    iterations++;

    sendEvent(200);
}


int main(int argc, const char* argv[])
{
    Dynamic_Mod_TestMarshal _app_handle;

    string ip = "127.0.0.1";
    int port = 15000;
    stdArgs( argc, argv, ip, port );

    int32 status = _app_handle.open();
    if (status)
    {
    WARN << "Failed to deploy query network";
    }
    else
    {
    _app_handle.sentEvent();
    _app_handle.runClient();
    }

    return status;
}
