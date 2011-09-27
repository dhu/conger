
#include "MedusaClient.h"
#include "Query.h"
#include "args.h"

using namespace Borealis;
using namespace NMSTL;
using namespace std;

Status print_event(ptr<StreamEvent> event)
{
    NOTICE << "Received event " << *event;
    return true;
}

int main(int argc, const char* argv[])
{

    string ip = "127.0.0.1";
    int port = 15000;

    MedusaClient client(InetAddress(ip,port));

    Query query("medusa://nms.lcs.mit.edu/simple/sensorquery-" + to_string(Time::now().to_secs()));

    string queryxml = string() +
        "<query name=\"test_query\">\n" +
        " <box name=\"my_map\" type=\"map\">\n" +
        "  <input port=\"1\" stream=\"output1\"/>\n" +
        "  <output port=\"1\" stream=\"output2\"/>\n" +
        "  <param name=\"output-field-name.0\" value=\"sensors.nodeid\"/>\n"  +
        "  <param name=\"expression.0\" value=\"sensors.nodeid\"/>\n"  +
        "  <param name=\"output-field-name.1\" value=\"sensors.light\"/>\n"  +
        "  <param name=\"expression.1\" value=\"sensors.light\"/>\n"  +
        "  <param name=\"output-field-name.2\" value=\"sensors.temp\"/>\n"  +
        "  <param name=\"expression.2\" value=\"sensors.temp\"/>\n"  +
        " </box>\n" +
        "</query>\n";

    RPC<void> r = client.create_query_xml(queryxml);

    if (!r.stat())
        FATAL << "Failed to create query: " << r.stat();

    r = client.start_query(query.get_name());
    if (!r.stat())
        FATAL << "Failed to start query: " << r.stat();

    NOTICE << "Subscribing to output stream";
    // Subscribing to receive the output on fast datapath
    NOTICE << "Subscribing";
    Status stat = client.set_data_handler(InetAddress(ip,port),wrap(&print_event));
    if ( stat )
    {
        stat = client.fast_subscribe("output2");
    }
    NOTICE << "Done subscribing";

    client.run();
}
