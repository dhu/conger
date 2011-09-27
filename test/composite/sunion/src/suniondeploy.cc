#include "HeadClient.h"
#include "Diagram.h"
#include "SUnionTestParams.h"

using namespace Borealis;

#define   MY_IP       "127.0.0.1"
#define   MY_PORT     "25000"

#define   HEAD_NODE   "localhost"

const string HOSTS[] = {    "127.0.0.1:17100", "127.0.0.1:17300",  "127.0.0.1:17500", "127.0.0.1:17700"};
const string REPLICAS[] = { "127.0.0.1:17200", "127.0.0.1:17400",  "127.0.0.1:17600", "127.0.0.1:17800"};

// --------------------------------------------------
// Produce the XML code for an SUnion from the given parameters
string union_box(string box_base, 
                 vector<string> input_streams,
                 vector<string> output_streams, 
                 string control_output_stream,                  
                 int delta_boundary, 
                 int stagger, 
                 int checkpoint_interval, 
                 int max_delay)
{

    // First box is the sunion
    string query_xml = string() +
        "    <box name=\"" + box_base + "/sunion\" type = \"sunion\">\n" +
        "      <parameter name=\"location\" value=\"input\"/>\n" +
        "      <parameter name=\"interval\" value=\"" + delta_boundary + "\"/>\n" +
        "      <parameter name=\"stagger\" value=\"" + stagger + "\"/>\n" +
        "      <parameter name=\"checkpoint\" value=\"" + checkpoint_interval + "\"/>\n";
        
    for ( int j = 0; j < (int)input_streams.size(); j++)
    { query_xml += "      <parameter name=\"max-delay." + to_string(j) + "\" value=\"" + max_delay + "\"/>\n";
    }

    for ( int i = 0; i < (int)input_streams.size(); i++)
    { query_xml += "      <in stream=\"" + input_streams[i] + "\"/>\n";
    }

    query_xml += "      <out stream=\"" + control_output_stream + "\"/>\n";

    for ( int k = 0; k < (int)output_streams.size(); k++)
    {    query_xml += "      <out stream=\"" + output_streams[k] + "\"/>\n";
    }

    query_xml += "    </box>\n";

    return query_xml;
}

// --------------------------------------------------
// Produce the XML code for an ExperimentalJoin from the given parameters
string self_join_box(string box_base, string input_stream, string output_stream,
                     int state_size, int checkpoint_interval)
              
{
    //if ( input_streams.size() != 2 )
    //{ FATAL << "Join requires 2 input streams";
    //}

    // The size of the left buffer determines how expensive the
    // computation is. Let's keep it at 1/2 or max 50 tuples
    int size_buffer_left =  (int)(0.5*state_size);
    int size_buffer_right =  state_size - size_buffer_left;

    // Second box is a self join box
    string query_xml = string() +
        "    <box name=\"" + box_base + "/join\" type = \"sjoin\">\n" +
        "      <parameter name=\"predicate\" value=\"left.value=right.value\"/>\n"  +

        // Ok to join in both directions because the first tuple to get processed has nothing
        // to join against!
        //"      <parameter name=\"leftjoin\" value=\"true\"/>\n"  +
        //"      <parameter name=\"rightjoin\" value=\"true\"/>\n"  +
        "      <parameter name=\"left-order-by\" value=\"TUPLES\"/>\n"  +
        "      <parameter name=\"right-order-by\" value=\"TUPLES\"/>\n"  +
        "      <parameter name=\"left-buffer-size\" value=\"" + size_buffer_left + "\"/>\n" +
        "      <parameter name=\"right-buffer-size\" value=\"" + size_buffer_right +  "\"/>\n" +
        // Get rid of all extra attibutes. Keep only data from left hand-side
        "      <parameter name=\"out-field.0\" value=\"left.value\" />\n" +
        "      <parameter name=\"out-field-name.0\" value=\"value\" />\n" +
        "      <in stream=\"" + input_stream + "\"/>\n" +
        "      <in stream=\"" + input_stream + "\"/>\n" +
        "      <out stream=\"" + output_stream + "\"/>\n" +
        "    </box>\n";

    return query_xml;
}


// --------------------------------------------------
// Produce the XML code for an SOutput operator from parameters
string soutput_box(string box_base, string input_stream,
                   string output_stream, string control_output_stream)
{

    // Third box is an soutput
    string query_xml = string() +
        "    <box name=\"" + box_base + "/soutput\" type = \"soutput\">\n" +
        "      <in stream=\"" + input_stream + "\"/>\n" +
        "      <out stream=\"" + control_output_stream + "\"/>\n" +
        "      <out stream=\"" + output_stream + "\"/>\n" +
        "    </box>\n";
    return query_xml;
}



////////////////////////////////////////////////////////////////////////////////
string create_one_query(SUnionTestParams& params, string query_base, vector<string> stream_names, string output_stream)
{

    string query_xml = string() + "  <query name=\"" + query_base +  "\">\n";  
        
    string box_base = query_base + "/box";

    vector<string> sunion_data_outputs;    
    sunion_data_outputs.push_back(query_base + "/Union");
    // No! Will use a self join.
    //sunion_data_outputs.push_back(query_base + "/Union2");

    string sunion_control_output = query_base + "/ControlUnion";

    int stagger = 0;
    if ( params._stagger_string == SUnionTestParams::STAGGER_STRING )
    { stagger = 1;
    }

    query_xml += union_box(box_base,stream_names,
                           sunion_data_outputs,
                           sunion_control_output,
                           params._delta_boundary,
                           stagger,
                           params._checkpoint_interval,
                           params._max_delay);


    // At least one box... SJoin needs more testing.
    //string join_output = query_base + "/Join";
    //query_xml += self_join_box(box_base,sunion_data_outputs[0],join_output,params._size_state,params._checkpoint_interval);
    
    //string soutput_control_output = query_base + "/ControlOutput";
    //query_xml += soutput_box(box_base,join_output,output_stream,soutput_control_output);

    string soutput_control_output = query_base + "/ControlOutput";
    query_xml += soutput_box(box_base,sunion_data_outputs[0],output_stream,soutput_control_output);


    query_xml += string("  </query>\n");
    return query_xml;

}


////////////////////////////////////////////////////////////////////////////////
string create_query(SUnionTestParams& params)
{

    int nb_nodes = params._nb_nodes;

    string query_xml = string("") +
        "<?xml version=\"1.0\"?>\n" +
        "<!DOCTYPE borealis SYSTEM \"../../../../src/src/borealis.dtd\">\n" +
        "\n" +
        "<borealis>\n" +
        "\n" +

        "  <input   stream=\"Input1\"        schema=\"TrivialTuple\"/>\n" +
        "  <input   stream=\"Input2\"        schema=\"TrivialTuple\"/>\n" +
        "  <input   stream=\"Input3\"        schema=\"TrivialTuple\"/>\n" +
        "  <output  stream=\"Output\"        schema=\"TrivialTuple\"/>\n";

    for ( int i = 1; i <= nb_nodes; i++ )
    {
        query_xml += 
            "  <output  stream=\"my_query" + to_string(i) + "/ControlUnion\"  schema=\"EmptyTuple\"  />\n" +
            "  <output  stream=\"my_query" + to_string(i) + "/ControlOutput\" schema=\"EmptyTuple\"  />\n";
    }

    query_xml +=
        string("\n") +
        "  <schema name=\"EmptyTuple\">\n" + 
        "  </schema>\n" +
        "\n" +        
        "  <schema name=\"TrivialTuple\">\n" +
        "    <field  name=\"value\"  type=\"int\" />\n" +
        "  </schema>\n";

    // First query
    vector<string> stream_names;
    stream_names.push_back("Input1");
    stream_names.push_back("Input2");
    stream_names.push_back("Input3");
    string final_output_stream("SOutput");

    string query_base("my_query1");
    string query_output = final_output_stream;
    if ( nb_nodes > 1 )
    { query_output = query_base + "/Output";
    }
    query_xml += create_one_query(params, query_base, stream_names, query_output);
    
    for ( int i = 2; i <= nb_nodes; i++ )
    {
        stream_names.clear();
        stream_names.push_back(query_output);
        query_base = string("my_query") + to_string(i);
        if ( i == nb_nodes )
        { query_output = final_output_stream; 
        }
        else
        { query_output = query_base + "/Output";
        }
        query_xml += create_one_query(params, query_base, stream_names, query_output);
    }

    query_xml += string() +
        "  <query name=\"proxy_query\">\n" + 
        "    <box name=\"fwd_filter\" type=\"filter\">\n" +
        "      <in   stream=\"SOutput\"/>\n" + 
        "      <out  stream=\"Output\" />\n" +
        "      <parameter name=\"expression.0\" value=\"true\"/>\n" +
        "</box>\n" +    
        "</query>\n";
        
    query_xml += string("</borealis>\n");
    return query_xml;

}

////////////////////////////////////////////////////////////////////////////////
string create_deployment(SUnionTestParams& params)
{

    int nb_nodes = params._nb_nodes;

    string query_xml = string("") +
        "<?xml version=\"1.0\"?>\n" +
        "<!DOCTYPE borealis SYSTEM \"../../../../src/src/borealis.dtd\">\n" +
        "\n" +
        "<deploy>\n" +
        "\n" +

        "  <publish    stream=\"Input1\"/>\n" + 
        "  <publish    stream=\"Input2\"/>\n" + 
        "  <publish    stream=\"Input3\"/>\n\n" + 
        "  <subscribe  stream=\"Output\" endpoint=\"" + MY_IP + ":" + MY_PORT + "\"/>\n\n";


    for ( int i = 1; i <= nb_nodes; i++)
    {
        if ( (params._replica_string == SUnionTestParams::REPLICA_STRING) )
        {
            query_xml += string("");
            query_xml += string("") + "  <replica_set  name=\"set" + to_string(i) +"\" query=\"my_query" + to_string(i) + "\" >\n";
            query_xml += string("") + "    <node  endpoint=\"" + HOSTS[i-1] + "\" />\n";
            query_xml += string("") + "    <node  endpoint=\"" + REPLICAS[i-1] + "\" />\n";
            query_xml += string("") + "  </replica_set>\n";
        }
        else
        {
            query_xml += "  <node endpoint=\"" + HOSTS[i-1] + "\"  query=\"my_query" + to_string(i) + "\" />\n";
        }
    }

    query_xml += "  <node endpoint=\"" + to_string(MY_IP) + ":18000\"  query=\"proxy_query\" />\n";

    query_xml += string("</deploy>\n");
    return query_xml;

}



////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  **argv )
{

    HeadClient  *client;
    RPC<void>  rpc;

    client = (HeadClient *)new HeadClient( InetAddress( HEAD_NODE,
                                                        DEFAULT_HEAD_PORT ));



    // No! Just submit the query, the other application will do all the deployment
    string query_file        = "Query.xml";
    string deployment_file   = "Deployment.xml";

    if ( argc > 1 )
    {
        query_file = "QueryVar.xml";
        deployment_file = "DeployVar.xml";

        // Initialize parameters for experiment from runtime arguments (which specify a file)
        SUnionTestParams parameters(argc,argv);
        string deployment_string = create_deployment(parameters);
        string query_string = create_query(parameters);

        ofstream fd(query_file.c_str());
        fd << query_string;
        fd.close();

        ofstream fd2(deployment_file.c_str());
        fd2 << deployment_string;
        fd2.close();

        NOTICE << "Query is\n " << query_string;
        NOTICE << "Deployment is\n " << deployment_string;
    }

    //rpc = client->deploy_xml_string( query ); --> Doesn't work?
    rpc = client->deploy_xml_file( query_file);
    NOTICE << "Submitted query description  " << rpc.stat();
    if ( rpc.valid() )
    {   NOTICE << "SUniontest succeeded.";
    }
    else
    {   NOTICE << "SUniontest failed:  " << rpc.stat();
    }

}
