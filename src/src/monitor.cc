
#include "MedusaClient.h"
#include "args.h"

using namespace Borealis;
using namespace NMSTL;

// --------------------------------------------------
double total_input_rate(vector<Stats> stats)
{

    double total = 0.0;
    for ( vector<Stats>::iterator i = stats.begin(); i != stats.end(); ++i)
    {
        total += (*i)._bw_in;
    }
    return total;

}


// --------------------------------------------------
int main(int argc, const char* argv[])
{

    // Parameters are a list of nodes to monitor IP:port IP:port, etc.
    map< string, ptr<MedusaClient> > nodes;

    if ( argc <= 1 )
            FATAL << "Please specify a list of valid IP:port for the nodes to monitor";

    for ( int i = 1; i < argc; i++)
    {
        string ip;
        int port;
        parse((char*)argv[i],ip,port);
        WARN << "Will monitor " << ip << ":" << port;
        try
        {
            ptr<MedusaClient> client(new MedusaClient(InetAddress(ip,port)));
            nodes[string(ip+":"+port)] = client;
        } catch (...)
        {
            FATAL << "Please specify a list of valid IP:port for the nodes to monitor";
        }
    }


    // Monitor nodes forever
    while ( true )
    {

        Thread::sleep(Time::msecs(1000));
        WARN << "Updating stats";

        // Update stats
        for ( map< string, ptr<MedusaClient> >::iterator i = nodes.begin(); i != nodes.end(); ++i)
        {
            string name = i->first;
            RPC< vector<Stats> > result = i->second->get_stats();
            if (!result.valid())
            {
                ERROR << result.stat();
            } else
            {
                vector<Stats> stats = *result;
                double input_rate = total_input_rate(stats);
                WARN << "Total input rate for node " << name << " is " << input_rate;
                WARN << stats.front()._cpu_usage;
            }
        }

    }

}
