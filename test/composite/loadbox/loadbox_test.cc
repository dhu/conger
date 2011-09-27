/**
   Note, this must be run from the BOREALISROOT/utility/test/simple/loadbox directory.

   Otherwise change the "box_path" variable appropriately to use an absolute path.

   -- yna.
*/

#include "MedusaClient.h"
#include "InputTuple.h"
#include "TupleHeader.h"

#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>

using namespace Borealis;

// Maximum size of buffer with data awaiting transmission to Borealis
int max_buffer = 64000;

// --------------------------------------------------
Status print_event(ptr<StreamEvent> event)
{

    // When testing, print content of tuples...
    // For each tuple
    int index = 0;
    for ( int i = 0; i < event->_inserted_count; i++)
    {

        index += HEADER_SIZE + sizeof(int);

        // Protocol field
    char* protocol = (char*) (&event->_bin_tuples[index]);
    string proto_string = string(protocol, PROT_SIZE);
    index += PROT_SIZE;

        NOTICE << "Protocol : " << proto_string;
    }
    return true;
}

// --------------------------------------------------
const int BATCH_SIZE = 10; // tuples
const int BATCH_TIME = 50; //msecs
const int SLEEP_TIME = 10; //msecs
const int NB_PROTOS = 4;
const string PROTOS[] = { string("dns"), string("smtp"), string("http"), string("ssh") };
// --------------------------------------------------

// Continuously produce data on input stream
class data_source
{
public:
    data_source(ptr<MedusaClient> client, string stream_name):
        _client(client), _stream_name(stream_name),
        _batch_size(BATCH_SIZE), _per_batch_time(BATCH_TIME), _sleep_time(SLEEP_TIME)
    {}

    void send_batch(Time start_time);
    void move_query();

private:
    ptr<MedusaClient> _client;
    string _stream_name;

    int _batch_size; // Nb tuples
    int _per_batch_time; // msecs between batches.
    int _sleep_time; // msecs
};


// --------------------------------------------------
void
DataSource::send_batch(Time start_time)
{

    Time current_time = Time::now();

    ptr<StreamEvent> event(new StreamEvent(_stream_name));
    event->_inject = true;

    // Send a batch of tuples at once
    int timestamp = (int)(current_time-start_time).to_secs();
    if ( timestamp < 0 ) timestamp = 0;
    for (int i = 0; i < _batch_size; i++)
    {
    int random_proto_index = rand() % NB_PROTOS;
    InputTuple tuple(timestamp,PROTOS[random_proto_index]);
    event->insert_bin(string((const char *)&tuple, sizeof(InputTuple)));
    }

    Status stat = _client->fast_post_event(event);
    if ( stat )
    {
    // All ok
    } else if ( stat.as_string() == DataHandler::NO_SPACE )
    {
    Thread::sleep(Time::msecs(_sleep_time)); // Wait if no more space in buffer
        WARN << "No space... dropping";
    } else
    {
    ERROR << "Connection closed... stopping event stream";
    return;
    }

    (new CallbackTimer(_client->get_loop(),wrap(this,&DataSource::send_batch,start_time)))->arm(Time::now()+Time::msecs(_per_batch_time));
}


int main(int argc, char** argv)
{
    string my_ip = "127.0.0.1";
    int my_port = 25000;
    string node1_ip = "127.0.0.1";
    int node1_port = 15000;

    ptr<MedusaClient> client(new MedusaClient(InetAddress(node1_ip, node1_port)));

    // Dynamically load the box.
    char cwd[1024];
    if ( getcwd(cwd, sizeof(cwd)) == NULL )
    FATAL << "Current directory exceeded buffer size...use Borealis from a shorter directory :)";

    string box_path = string(cwd) + "/.libs/libdfilter.so";
    AsyncRPC<void> ar = client->load_box(box_path);
    if ( ! ar.stat() )
    FATAL << "Failed to load box " << box_path;

    // Create a new query using the box.
    // Schema
    string schema_xml = string() +
    "<schema name=\"sand://cs.brown.edu/load_box_test/schema\">\n" +
        " <field name=\"start_time_sec\" type=\"int\"/>\n" +
        " <field name=\"protocol\"       type=\"string\" size=\"" + PROT_SIZE + "\"/>\n" +
    "</schema>\n";

    Schema schema;
    Status s = schema.from_xml(schema_xml);
    if ( ! s ) FATAL << "Failed parsing schema from xml : " << s;

    RPC<void> r = client->create_schema(schema);
    if ( !r.stat() ) FATAL << "Failed creating schema : " << r;

    // Streams.
    string input_stream_name = "sand://cs.brown.edu/load_box_test/istream";
    string output_stream_name = "sand://cs.brown.edu/load_box_test/ostream";

    StreamDef input_stream_def(input_stream_name, schema);
    r = client->create_stream(input_stream_def);
    if ( !r.stat() ) FATAL << "Failed creating stream: " << r;

    // Query hunk.
    string dbox_query_name = "sand://cs.brown.edu/load_box_test/query";
    string dbox_name = dbox_query_name + "/dynamic_box";
    string dbox_query = string() +
    "<query name=\"" + dbox_query_name + "\">\n" +
    "  <box name=\"" + dbox_name + "\" type=\"dfilter\">\n"+
    "    <input port=\"1\" stream=\"" + input_stream_name + "\"/>\n" +
    "    <output port=\"1\" stream=\"" + output_stream_name + "\"/>\n" +
    "    <param name=\"expression.0\" value=\"protocol = 'smtp'\"/>\n" +
    "    <param name=\"pass-on-false-port\" value=\"0\"/>\n" +
    "  </box>\n" +
    "</query>\n";

    // Create and start query.
    r = client->create_query_xml(dbox_query);
    if ( !r.stat() ) FATAL << "Failed creating query : " << r;

    r = client->start_query(dbox_query_name);
    if ( !r.stat() ) FATAL << "Failed starting query : " << r;

    // Subscribe.
    s = client->set_data_handler(InetAddress(my_ip, my_port), wrap(&print_event));
    if ( s )
    {
    client->fast_subscribe(output_stream_name);
    }

    // Generate input.
    s = client->set_data_path(max_buffer, node1_ip, node1_port);
    if ( ! s ) FATAL << "Unable to point data path to " << node1_ip << "@" << node1_port;
    data_source ds(client, input_stream_name);
    Time start_time = Time::now()-Time::msecs(100);
    ds.send_batch(start_time);
    client->run();

    return 0;

}
