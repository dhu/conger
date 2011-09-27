#include <NMSTL/rpc>
#include <NMSTL/debug>
#include <NMSTL/tcprpc>
#include <NMSTL/ntime>

#include <iostream>
#include <vector>

#include "rpc_QueryProcessor.h"
#include "MedusaClient.h"
#include "Statistics.h"
#include "TSStats.h"

using namespace Borealis;
using namespace NMSTL;
using namespace std;

int main(int argc, char* argv[])
{

    NMSTL::IOEventLoop loop;

    string ip = "127.0.0.1";
    int port = 38000;

    //------------------------------------------------
    //      TEST FOR STATISTICS
    //      ----------------------
    string ip_dest = "127.0.0.1";
    int port_dest = 15000;
    InetAddress rpc_dest(ip_dest, port_dest);


     MedusaClient* _client = new MedusaClient( InetAddress( ip, port ));

     Remote<QueryProcessor> r_qp = _client->qp_for(MedusaID(to_string(rpc_dest)));

     cout << "Got remote QP." << endl;

//     RPC<Statistics> s=r_qp.get_statistics();
//     cout << "Got statistics object." << endl;
//     cout<<s.as_string();

/*
    cout <<"***********************************************************************************************************"<< endl ;
     RPC<map <string, Statistics> > s1=r_qp.get_region_statistics();
     cout << "Got statistics object." << endl;
     map <string, Statistics> stats1 =*s1;
     if (stats1.size ()>0)
     {
     for (map <string, Statistics>::iterator c1 =stats1.begin();c1!=stats1.end();++c1)
     {
       cout << c1->first << " \n------\n" << c1->second ;
     }
     }
     else
     {

       cout << "No nodes" << endl;
     }
*/

    cout <<"***********************************************************************************************************"<< endl ;
     RPC<map <string, map <string, double> > > s2=r_qp.get_tuple_delivery_statistics();
     cout << "Got load shedder statistics object." << endl;
     map <string, map <string, double> > stats2 =*s2;
     if (stats2.size ()>0)
     {
     for (map <string, map <string,double> >::iterator c1 =stats2.begin();c1!=stats2.end();++c1)
     {
       cout << c1->first << " \n------" << c1->second ;
     }
     }
     else
     {

       cout << "No nodes" << endl;
     }



    cout <<"***********************************************************************************************************"<< endl ;
     cout << "Got CPU statistics:" <<  endl;
     RPC<map <string, double> > s3=r_qp.get_region_cpu_statistics();
     cout << "Got CPu statistics object." << endl;
     map <string, double> stats3 =*s3;
     if (stats3.size ()>0)
     {
     for (map <string, double>::iterator c1 =stats3.begin();c1!=stats3.end();++c1)
     {
       cout << c1->first << " \n------" << c1->second ;
     }
     }
     else
     {

       cout << "No nodes" << endl;
     }

      cout <<"***********************************************************************************************************"<< endl ;
   //  cout << "Got latency Stats statistics:" <<  endl;
     RPC<map <string, map <string, double> > > s4=r_qp.get_latency_statistics();
     cout << "Got latency statistics object." << endl;
     map <string, map <string, double> > stats4 =*s4;
     if (stats4.size ()>0)
     {
     for (map <string, map <string, double> >::iterator c1 =stats4.begin();c1!=stats4.end();++c1)
     {
       cout << c1->first << " \n------" << c1->second ;
     }
     }
     else
     {

       cout << "No nodes" << endl;
     }



/*    string ip_dest2 = "127.0.0.1";
    int port_dest2 = 20000;
    InetAddress rpc_dest2(ip_dest2, port_dest2);

     Remote<QueryProcessor> r_qp2 = _client->qp_for(MedusaID(to_string(rpc_dest2)));

     cout << "Got remote QP." << endl;

     RPC<Statistics> s2=r_qp2.get_statistics();
     cout << "Got statistics object." << endl;
     cout<<s2.as_string();

     RPC<double> s2_2=r_qp.get_Cpu_statistics();

     cout << "Got CPU statistics:" << s2_2<< endl;

    cout <<"***********************************************************************************************************"<< endl ;
*/

     //---------------------------------------
     //OLD RPC TEST

     // Create an endpoint capable of communicating with the server
    //  NMSTL::TCPRPCEndpoint my_endpoint(loop, NMSTL::TCPSocket(NMSTL::InetAddress(ip,port)));

    // Get a handle to the remote calculator
    // NMSTL::Remote<Borealis::QOSOptimizer> optimizer = my_endpoint.get<Borealis::QOSOptimizer>("QOSOptimizer");

    // Perform an operation
    // NMSTL::RPC<void> result = optimizer.do_update(1.0, 1.0);

    //    if ( !result.valid())
    //        cout << "Failure: " << result.stat();
    //    else
    //        cout << "Operation succeeded " << result << endl;

}

