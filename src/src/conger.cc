#include "common.h"
#include "args.h"

/// activates the compiler directive for HA
#define INCLUDE_HA 

#ifdef INCLUDE_HA
#include "HA.h"
#endif 

#include "AvailabilityMonitor.h"
#include "LocalLoadShedder.h"
#include "NHLoadShedder.h"
#include "ContractBasedOptimizer.h"
#include "QOSOptimizer.h"
#include "QueryProcessor.h"

#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>

#include <csignal>

BOREALIS_NAMESPACE_BEGIN

int     port_base = DEFAULT_BOREALIS_PORT;
string  ip        = "localhost";

int     nh_load_shedder_mode = 0;

ptr<QueryProcessor>  my_qp;


/// Structure internal to this file containing the actual Medusa components.
///
struct medusa_components
{
    IOEventLoop             &_loop;
    InetAddress              _addr;
    ptr<AvailabilityMonitor>  _avail_mon;
    ptr<LocalLoadShedder>     _local_ls;
    ptr<NHLoadShedder>        _nh_ls;
    ptr<NHOptimizer>          _nhopt;
    ptr<QOSOptimizer>         _qosopt;
    ptr<QueryProcessor>       _qp;

    typedef vector< ptr<BasicComponent> > Components;

    Components _all;                    // Pointers to all the components

    medusa_components(IOEventLoop      &loop,
                      InetAddress       addr,
                      InetAddress       data_addr)
        : _loop(loop),
          _addr(addr),

          _avail_mon( ptr<AvailabilityMonitor>(new AvailabilityMonitor(
                                                           to_string(_addr)))), 
                                                                        
          _local_ls(ptr<LocalLoadShedder>(new LocalLoadShedder(
                                                           to_string(_addr)))),
          _nh_ls(ptr<NHLoadShedder>(new NHLoadShedder(to_string(_addr), 
                     NHLoadShedder::NHLoadShedderMode(nh_load_shedder_mode)))),

          _nhopt(ptr<NHOptimizer>(new ContractBasedOptimizer(
                                                           to_string(_addr)))),

          _qosopt(ptr<QOSOptimizer>(new QOSOptimizer(to_string(_addr)))),

          _qp( ptr<QueryProcessor>(new QueryProcessor(to_string(_addr), 
                                                                   data_addr)))
    {
        _all.push_back(_avail_mon);
        _all.push_back(_local_ls);
        _all.push_back(_nh_ls);
        _all.push_back(_nhopt);
        _all.push_back(_qosopt);
        _all.push_back(_qp);

        for (Components::const_iterator i = _all.begin(); i != _all.end(); ++i)
        {
            BasicComponent& component = **i;
            
            component._avail_mon_client = component.endpoint().client_for(
                                              _avail_mon->endpoint());

            component._avail_mon = component._avail_mon_client->
                                get<AvailabilityMonitor>("AvailabilityMonitor");

            component._local_ls_client =  component.endpoint().client_for(
                                                        _local_ls->endpoint());

            component._local_ls = component._local_ls_client->
                                get<LocalLoadShedder>("LocalLoadShedder");

            component._nh_ls_client =  component.endpoint().client_for(
                                                        _nh_ls->endpoint());

            component._nh_ls = component._nh_ls_client->
                                get<NHLoadShedder>("NHLoadShedder");

            component._nhopt_client =  component.endpoint().client_for(
                                                        _nhopt->endpoint());

            component._nhopt = component._nhopt_client->
                                        get<NHOptimizer>("NHOptimizer");

            component._qosopt_client = component.endpoint().client_for(
                                                        _qosopt->endpoint());

            component._qosopt = component._qosopt_client->
                                        get<QOSOptimizer>("QOSOptimizer");

            component._qp_client = component.endpoint().client_for(
                                                        _qp->endpoint());

            component._qp = component._qp_client->
                                        get<QueryProcessor>("QueryProcessor");

            component._addr      = _addr;
            component._port_base = port_base;

            component._loop = &_loop;
        }

        for (Components::const_iterator i = _all.begin(); i != _all.end(); ++i)
        {
            (*i)->init();
        }
    }

    /// If any component has failed, returns its status; else true.
    ///
    Status stat() const
    {
        for (Components::const_iterator i = _all.begin(); i != _all.end(); ++i)
        {
            if (!(*i)->stat())
            {   
                return((*i)->stat());
            }
        }

        return(true);
    }

    /// Starts all component threads.
    ///
    void start()
    {
        for (Components::const_iterator i = _all.begin(); i != _all.end(); ++i)
        {
            (*i)->start();
        }
    }

    /// Terminates all component threads.
    ///
    void terminate()
    {
        for (Components::const_iterator i = _all.begin(); i != _all.end(); ++i)
        {
            (*i)->terminate();
        }
    }

    /// Joins all component threads.
    ///
    void join()
    {
        for (Components::const_iterator i = _all.begin(); i != _all.end(); ++i)
        {
            (*i)->join();
        }
    }
}; // end medusa_components

////////////////////////////////////////////////////////////////////////////////
//
// signal handler for SIGTERM.
//
void term(int sig)
{
    if (my_qp)
    {
        my_qp->output_stats();
    }
    exit(0);
}

////////////////////////////////////////////////////////////////////////////////
//
int main(int argc, const char *argv[])
{
    string checkpoint_method = "whole"; 
    int    checkpoint_interval = -1;
    int    checkpoint_slack = 50;
    int    run_time  = INIT;

    vector<MedusaID> ha_partners;

    signal(SIGTERM, term);

    stdArgs(argc, argv,
            ip, port_base,
            checkpoint_method, 
            checkpoint_interval, 
            checkpoint_slack, 
            ha_partners, 
            &nh_load_shedder_mode,
            &run_time);

    DEBUG << "nh_load_shedder_mode = " << nh_load_shedder_mode;

    ip = Util::get_host_address(ip);

    WARN << "Starting borealis at address: " << ip << ":" << port_base;

    // We get most of our parameters through the optarg method above
    // These are a few extras
    for (int i = 0; i < argc; i++)
    {
        string option(argv[i]);

        if (option == "decremental")
        {
            HA::set_decremental_checkpoint_interval();
            NOTICE << "We will decrease the checkpoint interval over time!";
        }
        else if (option == "rb")
        {
            HA::random_backup_distribution = true;
            NOTICE << "random backup distribution";
        }
        else try 
        {
            HAMethod HA_method = string_to_method(option);
            HA::set_HA_method(HA_method);
        } catch (AuroraBadEntityException e) {}
    }

#ifdef INCLUDE_HA
    if (HA::get_HA_method() != NO_HA)   // now HA is on.
    {
        NOTICE << "HA is on.";
        HA::set_checkpoint_method(string_to_checkpoint_method(
                                                    checkpoint_method));
        HA::set_checkpoint_interval(checkpoint_interval);
        HA::set_checkpoint_slack(checkpoint_slack);
        for (unsigned register int i = 0; i < ha_partners.size(); i++)
        {
            HA::add_ha_partner(ha_partners[i]);
        }
    }
#endif

    Debug::get().increase_verbosity_to("xmlrpc", DebugMsg::info);

    NOTICE << "Creating components...";

    IOEventLoop loop;
    medusa_components components(loop,
                                 ip + ":" + to_string(port_base),
                                 ip + ":" + to_string(port_base 
                                                       + DATA_PORT_OFFSET));

    if (!components.stat())
    {   
        FATAL << "Unable to create components: " << components.stat();
    }

    my_qp = components._qp;
    NOTICE << "Creating RPC proxies...";

    // A proxy forwards events received in one thread (this thread, i.e.,
    // the main event loop thread) to a separate thread (the m_admin or
    // m_qp thread).  That way we can isolate components from
    // network stuff.
    //
    RPCProxy proxy(loop);

    proxy.set_tunnel("AvailabilityMonitor", components._avail_mon->endpoint());
    proxy.set_tunnel("LocalLoadShedder",    components._local_ls->endpoint());
    proxy.set_tunnel("NHLoadShedder",       components._nh_ls->endpoint());
    proxy.set_tunnel("NHOptimizer",         components._nhopt->endpoint());
    proxy.set_tunnel("QOSOptimizer",        components._qosopt->endpoint());
    proxy.set_tunnel("QueryProcessor",      components._qp->endpoint());

    TCPRPCProxy tacceptor(loop, components._addr, proxy);

    if (!tacceptor)
    {   
        FATAL << "Unable to accept TCP/RPC connections: " << tacceptor.stat();
    }

    NOTICE << "Accepting TCP/RPC connections on "
           << tacceptor.get_socket().getsockname();

    XMLRPCProxy xacceptor(loop,
                          InetAddress(components._addr.get_host(),
                                      port_base + XML_RPC_PORT_OFFSET),
                          proxy);

    if (!xacceptor)
    {   
        FATAL << "Unable to accept XML/RPC connections: " << xacceptor.stat();
    }

    NOTICE << "Accepting XML/RPC connections on "
           << xacceptor.get_socket().getsockname();

    NOTICE << "Don't forget to use \"-d " << ip << ":" << port_base
           << "\" in clients";

    // If an exit time has been specified
    if (run_time != INIT)
    {
        (new CallbackTimer(loop, wrap(&loop, &IOEventLoop::terminate)))->
                     arm(Time::now() + Time::msecs(run_time));
    }

    components.start();

    // Start a terminal handler (right now, the only useful command is exit!)
    //TermHandler term( loop,
    //                  "Medusa " + to_string( components.m_addr ) + "> " );

    // Doesn't return until you type "exit" at the prompt
    loop.run();

    // Signal components to quit (returns immediately)
    components.terminate();

    NOTICE << "Waiting for components to exit...\n\n";

    components.join();

    NOTICE << "All components have exited.\n\n";

    return 0;
}

BOREALIS_NAMESPACE_END

// --------------------------------------------------
int main(int argc, const char *argv[])
{
    Borealis::main(argc, argv);
}
