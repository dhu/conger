
#include "MedusaClient.h"
#include "args.h"
#include "InputTuple.h"
#include "TupleHeader.h"

#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>

using namespace Borealis;
using namespace NMSTL;


// --------------------------------------------------
Status print_event(ptr<StreamEvent> event)
{

    // When testing, print content of tuples...
    // For each tuple
    int index = 0;
    for ( int i = 0; i < event->m_inserted_count; i++)
    {

        index += HEADER_SIZE;

        // Ordered_on
        int begin_time_interval = *(int *)(&event->m_bin_tuples[index]);
        index += sizeof(int);

        // Aggregate value
        int count = *(int *)(&event->m_bin_tuples[index]);
        index += sizeof(int);

        NOTICE << "For time interval starting at "
               << begin_time_interval << " tuple count was " << count;
    }
    return true;
}

// --------------------------------------------------
const int BATCH_SIZE = 10; // tuples
const int BATCH_TIME = 50; //msecs
const int SLEEP_TIME = 10; //msecs
const int NB_PROTOS = 5;
const string PROTOS[] = { string("dns"), string("smtp"), string("http"), string("ssh"), string("sand") };
const int BATCH_MOVE = 1000; // 10 seconds between moving query (= BATCH_MOVE / BATCH_TIME).


// Two well-known Borealis nodes. We connect to the first.
string node1_ip = "127.0.0.1";
int node1_port = 15000;

string node2_ip = "127.0.0.1";
int node2_port = 20000;

// Maximum size of buffer with data awaiting transmission to Borealis
int max_buffer = 64000;


// Continuously produce data on input stream
class DataSource
{
public:
    DataSource(ptr<MedusaClient> client, string stream_name, string query_name):
        m_client(client), m_stream_name(stream_name), m_query_name(query_name),
        m_batch_size(BATCH_SIZE), m_per_batch_time(BATCH_TIME), m_sleep_time(SLEEP_TIME),
    m_nb_batch_until_move(BATCH_MOVE), m_query_cur_loc(0)
    {
    }

    void send_batch(Time start_time);

private:
    ptr<MedusaClient> m_client;
    string m_stream_name;
    string m_query_name;

    int m_batch_size; // Nb tuples
    int m_per_batch_time; // msecs between batches.
    int m_sleep_time; // msecs
    int m_nb_batch_until_move;
    int m_query_cur_loc;
};

// --------------------------------------------------
void
DataSource::send_batch(Time start_time)
{

    Time current_time = Time::now();

    ptr<StreamEvent> event(new StreamEvent(m_stream_name));
    event->m_inject = true;

    // Send a batch of tuples at once
    int timestamp = (int)(current_time-start_time).to_secs();
    if ( timestamp < 0 ) timestamp = 0;
    for (int i = 0; i < m_batch_size; i++)
    {
    int random_proto_index = rand() % NB_PROTOS;
    InputTuple tuple(timestamp,PROTOS[random_proto_index]);
    event->insert_bin(string((const char *)&tuple, sizeof(InputTuple)));
    }

    Status stat = m_client->fast_post_event(event);
    if ( stat )
    {
    // All ok
    m_nb_batch_until_move--;
    } else if ( stat.as_string() == DataHandler::NO_SPACE )
    {
    Thread::sleep(Time::msecs(m_sleep_time)); // Wait if no more space in buffer
        WARN << "No space... dropping";
    } else
    {
    ERROR << "Connection closed... stopping event stream";
    return;
    }

    if (m_nb_batch_until_move <= 0)
    {

    // -- Move query ------------------------------------
    string dest_ip = (m_query_cur_loc)? node1_ip : node2_ip;
    int dest_port = (m_query_cur_loc)? node1_port : node2_port;

    string src_ip = (m_query_cur_loc)? node2_ip: node1_ip;
    int src_port = (m_query_cur_loc)? node2_port : node1_port;

    InetAddress query_src(src_ip, src_port);
    InetAddress query_dest(dest_ip, dest_port);

    NOTICE << "Moving the query from " << to_string(query_src) << " to: " << to_string(query_dest);

    Remote<Admin> r_admin = m_client->admin_for(MedusaID(to_string(query_src)));

    NOTICE << "Moving box by box...";

    Name split_test1 = m_query_name + "/split_test1";
    Name split_test2 = m_query_name + "/split_test2";
    Name split_test3 = m_query_name + "/split_test3";
    Name split_test4 = m_query_name + "/split_test4";
    Name box_name1 = m_query_name + "/filtersandbox";
    Name box_name2 = m_query_name + "/filtersmtpbox";
    Name box_name3 = m_query_name + "/unionbox";
    Name box_name4 = m_query_name + "/aggbox";

    map<Name, vector<Name> > move_hunks;
    if (dest_ip == node2_ip && dest_port == node2_port)
    {
        move_hunks[split_test1] = vector<Name>(1,box_name1);
        move_hunks[split_test2] = vector<Name>(1,box_name2);
        move_hunks[split_test3] = vector<Name>(1,box_name3);
        move_hunks[split_test4] = vector<Name>(1,box_name4);
    } else
    {
        move_hunks[split_test3] = vector<Name>(1,box_name3);
    }

    RPC<void> r = r_admin.split_query(m_query_name, move_hunks);

    //r = r_admin.move_load(vector<Name>(1,split_test1), to_string(query_dest));
    //r = r_admin.move_load(vector<Name>(1,split_test2), to_string(query_dest));
    r = r_admin.move_load(vector<Name>(1,split_test3), to_string(query_dest));
    //r = r_admin.move_load(vector<Name>(1,split_test4), to_string(query_dest));

    r_admin = m_client->admin_for(MedusaID(to_string(query_dest)));

    vector<Name> merge_splits;
    if (dest_ip == node2_ip && dest_port == node2_port)
    {
        merge_splits.push_back(split_test3);
    } else
    {
        merge_splits.push_back(split_test1);
        merge_splits.push_back(split_test2);
        merge_splits.push_back(split_test3);
        merge_splits.push_back(split_test4);
    }

    r = r_admin.merge_query(merge_splits, m_query_name);

    NOTICE << "Moved query: " << r;

    //if (! m_client->set_data_path(max_buffer,dest_ip,dest_port))
    {
    //    FATAL << "Failed setting data path";
    //}
    //NOTICE << "Set data path to: " << dest_ip << ":" << dest_port;

    m_query_cur_loc = (m_query_cur_loc + 1) % 2;
    m_nb_batch_until_move = BATCH_MOVE;

    }
    (new CallbackTimer(m_client->get_loop(),wrap(this,&DataSource::send_batch,start_time)))->arm(Time::now()+Time::msecs(m_per_batch_time));

}





int main(int argc, const char* argv[])
{

    // My own information
    string my_ip = "127.0.0.1";
    int my_port = 25000;

    stdArgs(argc,argv,node1_ip,node1_port);

    // Client for Borealis node
    ptr<MedusaClient> client(new MedusaClient(InetAddress(node1_ip,node1_port)));


    // -- Create schema -----------------------------------
    NOTICE << "Creating Aurora schema";
    string schema_xml = string() +
        "<schema name=\"medusa://nms.lcs.mit.edu/ids/packet_header_schema\">\n" +
        " <field name=\"start_time_sec\" type=\"int\"/>\n" +
        " <field name=\"protocol\" type=\"string\" size=\"" + PROT_SIZE + "\"/>\n" +
        "</schema>\n";

    Schema schema;
    Status s = schema.from_xml(schema_xml);
    if ( !s ) FATAL << "Failed parsing schema from xml" << s;

    RPC<void> r = client->create_schema(schema);
    NOTICE << "create_schema: " << r;
    if (!r.stat()) return 1;



    // -- Create input streams ---------------------------
    NOTICE << "Creating input streams";
    string input_stream_name = "medusa://nms.lcs.mit.edu/ids/packet_header/" + my_ip + ":" + my_port;
    StreamDef input_stream_def(input_stream_name, schema);
    r = client->create_stream(input_stream_def);
    NOTICE << "create_stream: " << r;
    if (!r.stat()) return 1;



    // -- Create query network ---------------------------
    NOTICE << "Creating the query";

    string output_stream_base = "medusa://nms.lcs.mit.edu/ids/";

    string query_name = "medusa://nms.lcs.mit.edu/ids/simple_query/" + my_ip + ":" + my_port;

    string filter_sand_box_name = query_name + "/filtersandbox";
    string filter_sand_box_output_name = output_stream_base + "raw_sand_filtered/" + my_ip + ":" + my_port;
    string filter_sand_box_output2_name = output_stream_base + "raw_sand_dump/" + my_ip + ":" + my_port;
    string filter_sand_box_expr = "protocol = 'sand'";

    string filter_smtp_box_name = query_name + "/filtersmtpbox";
    string filter_smtp_box_output_name = output_stream_base + "raw_smtp_filtered/" + my_ip + ":" + my_port;
    string filter_smtp_box_output2_name = output_stream_base + "raw_smtp_dump/" + my_ip + ":" + my_port;
    string filter_smtp_box_expr = "protocol = 'smtp'";

    string union_box_name = query_name + "/unionbox";
    string union_box_output_name = output_stream_base + "unioned_packets/"+ my_ip + ":"+ my_port;

    string aggregate_box_name = query_name + "/aggbox";
    string aggregate_box_output_name = output_stream_base + "suspicious/" + my_ip + ":" + my_port;

    string query_xml = string() +

        "<query name=\"" + query_name + "\">\n" +

    // Filter all 'sand' packets
    " <box name=\"" + filter_sand_box_name + "\" type = \"filter\">\n" +
    "  <input port=\"1\" stream=\"" + input_stream_name + "\"/>\n" +
    "  <output port=\"1\" stream=\"" + filter_sand_box_output_name + "\"/>\n" +
    "  <output port=\"2\" stream=\"" + filter_sand_box_output2_name + "\"/>\n" +
    "  <param name=\"expression.0\" value=\"" + filter_sand_box_expr + "\"/>\n" +
    "  <param name=\"pass-on-false-port\" value=\"1\"/>\n" +
    " </box>\n" +

    // Filter all 'smtp' packets
    " <box name=\"" + filter_smtp_box_name + "\" type = \"filter\">\n" +
    "  <input port=\"1\" stream=\"" + input_stream_name + "\"/>\n" +
    "  <output port=\"1\" stream=\"" + filter_smtp_box_output_name + "\"/>\n" +
    "  <output port=\"2\" stream=\"" + filter_smtp_box_output2_name + "\"/>\n" +
    "  <param name=\"expression.0\" value=\"" + filter_smtp_box_expr + "\"/>\n" +
    "  <param name=\"pass-on-false-port\" value=\"1\"/>\n" +
    " </box>\n" +

    // combine the packets into a single stream
    " <box name=\"" + union_box_name + "\" type = \"union\">\n" +
    "  <input port=\"1\" stream=\"" + filter_sand_box_output_name + "\"/>\n" +
    "  <input port=\"2\" stream=\"" + filter_smtp_box_output_name + "\"/>\n" +
    "  <output port=\"1\" stream=\"" + union_box_output_name + "\"/>\n" +
    " </box>\n" +

        // For every 1 seconds count the number of packets
        " <box name=\""  + aggregate_box_name +  "\" type=\"aggregate\">\n" +
        //"  <input port=\"1\" stream=\"" + filter_sand_box_output_name + "\"/>\n" +
        "  <input port=\"1\" stream=\"" + union_box_output_name + "\"/>\n" +
        "  <output port=\"1\" stream=\"" + aggregate_box_output_name + "\"/>\n" +
        "  <param name=\"aggregate-function.0\" value=\"count()\"/>\n"  +
        "  <param name=\"window-size-by\" value=\"VALUES\"/>\n"  +
        "  <param name=\"window-size\" value=\"1\"/>\n"  +
        "  <param name=\"advance\" value=\"1\"/>\n"  +
        "  <param name=\"order-by\" value=\"FIELD\"/>\n"  +
        "  <param name=\"order-on-field\" value=\"start_time_sec\"/>\n"  +
        " </box>\n" +

        "</query>\n";

    r = client->create_query_xml(query_xml);
    NOTICE << "create_query_xml: " << r;
    if (!r.stat()) return false;
    NOTICE << "Starting queries";
    r = client->start_query(query_name);
    NOTICE << "start_query: " << r;
    if (!r.stat()) return false;


    // -- Subscribe to output ---------------------------
    NOTICE << "Subscribing";
    Status stat = client->set_data_handler(InetAddress(my_ip,my_port),wrap(&print_event));
    if ( stat )
    {
        stat = client->fast_subscribe(aggregate_box_output_name);
    }
    NOTICE << "Done subscribing";

    // -- Start generating input ------------------------
    if (! client->set_data_path(max_buffer,node1_ip,node1_port))
    {
        FATAL << "Failed setting data path";
    }
    NOTICE << "Set data path";
    DataSource ds(client,input_stream_name, query_name);
    Time start_time = Time::now()-Time::msecs(100);
    ds.send_batch(start_time);
    client->run();

    return 0;


}
