
#include "MedusaClient.h"
#include "args.h"

#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>

using namespace Borealis;
using namespace NMSTL;

const int PROT_SIZE = 4;

// Data structure representing the tuples that we are going to send
struct PacketTuple : public TupleHeader
{

    int32   m_start_time_sec;
    char    m_protocol[ PROT_SIZE ];

    PacketTuple(int start_time_sec, string protocol)
        : TupleHeader(), m_start_time_sec(start_time_sec)
    {
        memset(&this->m_protocol, 0, sizeof this->m_protocol);
        strncpy(this->m_protocol, protocol.c_str(), sizeof this->m_protocol);
    }

} __attribute__((__packed__));


/*
 * Simple example of a Borealis client that uses
 * the fast datapath
 */


// --------------------------------------------------
Status print_event(ptr<StreamEvent> event)
{

    // When testing, print content of tuples...
    // For each tuple
    int index = 0;
    for ( int i = 0; i < event->_inserted_count; i++)
    {

        index += HEADER_SIZE;

        // Ordered_on
        int begin_time_interval = *(int *)(&event->_bin_tuples[index]);
        index += sizeof(int);

        // Aggregate value
        int count = *(int *)(&event->_bin_tuples[index]);
        index += sizeof(int);

        NOTICE << "For time interval starting at "
               << begin_time_interval << " tuple count was " << count;
    }
    return true;
}

// --------------------------------------------------
const int BATCH_SIZE = 20; // tuples
const int SLEEP_TIME = 100; //msecs
const int NB_PROTOS = 4;
const string PROTOS[] = { string("dns"), string("smtp"), string("http"), string("ssh") };

// Continuously produce data on input stream
//class DataSource : public Thread
//{
class DataSource
{
public:
    DataSource(ptr<MedusaClient> client, string stream_name):
        m_client(client), m_stream_name(stream_name),
        m_batch_size(BATCH_SIZE), m_sleep_time(SLEEP_TIME)
        {
    }

    void send_batch(Time start_time);

private:
    ptr<MedusaClient> m_client;
    string m_stream_name;

    int m_batch_size; // Nb tuples
    int m_sleep_time; // msecs


};

// --------------------------------------------------
void
DataSource::send_batch(Time start_time)
{

    Time current_time = Time::now();

    ptr<StreamEvent> event(new StreamEvent(m_stream_name));
    event->_inject = true;

    // Send a batch of tuples at once
    int timestamp = (int)(current_time-start_time).to_secs();
    if ( timestamp < 0 ) timestamp = 0;
    for (int i = 0; i < m_batch_size; i++)
    {
        int random_proto_index = rand() % NB_PROTOS;
        PacketTuple tuple(timestamp,PROTOS[random_proto_index]);
        event->insert_bin(string((const char *)&tuple, sizeof(PacketTuple)));
    }

    Status stat = m_client->fast_post_event(event);
    if ( stat )
    {
        // All ok
    } else if ( stat.as_string() == DataHandler::NO_SPACE )
    {
        Thread::sleep(Time::msecs(m_sleep_time)); // Wait if no more space in buffer
    } else
    {
        ERROR << "Connection closed... stopping event stream";
        return;
    }
    (new CallbackTimer(m_client->get_loop(),wrap(this,&DataSource::send_batch,start_time)))->arm(Time::now()+Time::msecs(m_sleep_time));

}


// --------------------------------------------------
int main(int argc, const char* argv[])
{

    // Known Borealis node
    string ip = "127.0.0.1";
    int port = 15000;

    // My own information
    string my_ip = "127.0.0.1";
    int my_port = 25000;

    stdArgs(argc,argv,ip,port);

    // Maximum size of buffer with data awaiting transmission to Borealis
    int max_buffer = 64000;

    // Client for Borealis node
    ptr<MedusaClient> client(new MedusaClient(InetAddress(ip,port)));

    // --------------------------------------------------
    NOTICE << "Creating Aurora schema";
    string schema_name = string() + "medusa://nms.lcs.mit.edu/ids/packet_header_schema";
    string schema_xml = string() +
        "<schema name=\"" + schema_name + "\">\n" +
        " <field name=\"start_time_sec\" type=\"int\"/>\n" +
        " <field name=\"protocol\" type=\"string\" size=\"" + PROT_SIZE + "\"/>\n" +
        "</schema>\n";
    //Schema schema;
    //Status s = schema.from_xml(schema_xml);
    //if ( !s )
    //    FATAL << "Failed parsing schema from xml" << s;

    //RPC<void> r = client->create_schema(schema);  // Deprecated API.
    RPC<void> r = client->add_schema_xml(schema_xml);
    NOTICE << "create_schema: " << r;
    if (!r.stat()) FATAL << "Failed creating a schema " << r;


    // --------------------------------------------------
    NOTICE << "Creating input streams";
    string input_stream_name = "medusa://nms.lcs.mit.edu/ids/packet_header/" + my_ip + ":" + my_port;

    string input_stream_xml  = "<stream name=" + input_stream_name
                                  + " schema=" + schema_name +  "\" />\n";

    //StreamDef input_stream_def(input_stream_name, schema);
    //r = client->create_stream(input_stream_def);   // Deprecated API.
    r = client->add_stream_xml(input_stream_xml);
    NOTICE << "create_stream: " << r;
    if (!r.stat()) return 1;

    // --------------------------------------------------
    NOTICE << "Creating the query";
    string output_stream_name = "medusa://nms.lcs.mit.edu/ids/suspicious/" + my_ip + ":" + my_port;
    string query_name = "medusa://nms.lcs.mit.edu/ids/simple_query/" + my_ip + ":" + my_port;
    string box_name = query_name + "/box";

    string query_xml = string() +
        "<query name=\"" + query_name + "\">\n" +

        // For every 1 seconds count the number of packets
        " <box name=\""  + box_name +  "\" type=\"aggregate\">\n" +
        "  <in    stream=\"" + input_stream_name + "\"/>\n" +
        "  <out   stream=\"" + output_stream_name + "\"/>\n" +
        "  <parameter name=\"aggregate-function.0\" value=\"count()\"/>\n"  +
        "  <parameter name=\"window-size-by\" value=\"VALUES\"/>\n"  +
        "  <parameter name=\"window-size\" value=\"1\"/>\n"  +
        "  <parameter name=\"advance\" value=\"1\"/>\n"  +
        "  <parameter name=\"order-by\" value=\"FIELD\"/>\n"  +
        "  <parameter name=\"order-on-field\" value=\"start_time_sec\"/>\n"  +
        " </box>\n" +

        "</query>\n";

    //r = client->create_query_xml(query_xml);  // Deprecated API.
    r = client->add_query_xml(query_xml);
    NOTICE << "create_query_xml: " << r;
    if (!r.stat()) return false;
    NOTICE << "Starting queries";
    r = client->start_query(query_name);
    NOTICE << "start_query: " << r;
    if (!r.stat()) return false;

    // --------------------------------------------------
    // Subscribing to receive the output on fast datapath
    NOTICE << "Subscribing";
    Status stat = client->set_data_handler(InetAddress(my_ip,my_port),wrap(&print_event));
    if ( stat )
    {
        stat = client->fast_subscribe(output_stream_name);
    }
    NOTICE << "Done subscribing";

    // --------------------------------------------------
    // Starting to produce events on input stream
    if (! client->set_data_path(max_buffer,ip,port))
    {
        FATAL << "Failed setting data path";
    }
    NOTICE << "Set data path";
    DataSource ds(client,input_stream_name);
    Time start_time = Time::now()-Time::msecs(100);
    ds.send_batch(start_time);
    client->run();

    return 0;

}



