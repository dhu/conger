#ifndef THREADCLIENT_H
#define THREADCLIENT_H

#include <map>

#include "rpc_QueryProcessor.h"
#include "rpc_LocalLoadShedder.h"
#include "rpc_NHLoadShedder.h"

#include <NMSTL/thread>
#include <NMSTL/tcprpc>

BOREALIS_NAMESPACE_BEGIN;

class ThreadClient :
    private Remote<QueryProcessor>
{
    // Internal classes.
    template<typename T, typename TArg>
    class ThreadAcceptor
    {
        ptr<TCPSocket> _listener;
        map<InetAddress, ptr<Threaded<T> > > _clients;

        TArg _arg;
        bool _terminated;

      public:

        ThreadAcceptor(ptr<TCPSocket> l, TArg a) :
            _listener(l), _arg(a), _terminated(false)
        {}

        ~ThreadAcceptor() {}

        void run()
        {
            while (!_terminated)
            {
                DEBUG << "Calling accept...";

                InetAddress peer_addr;
                Socket client = _listener->accept(peer_addr);

                if ( _terminated ) return;

                DEBUG << "Accepted client from " << peer_addr;

                _clients[peer_addr] =
                    ptr<Threaded<T> >(new Threaded<T>(client, _arg));

                _clients[peer_addr]->start();
                _clients[peer_addr]->detach();
            }
        }

        ptr<T> get_client(InetAddress addr)
        {
            typename map<InetAddress, ptr<Threaded<T> > >::iterator
                found_client = _clients.find(addr);

            if ( found_client != _clients.end() )
                return found_client->second;

            return ptr<T>();
        }

        void terminate()
        {
            _terminated = true;
            ::shutdown(_listener->get_fd(), SHUT_RD);

            typename map<InetAddress, ptr<Threaded<T> > >::iterator it =
                _clients.begin();
            typename map<InetAddress, ptr<Threaded<T> > >::iterator end =
                _clients.end();

            for (; it != end; ++it)
                it->second->terminate();
        }
    };


    template<typename Arg>
    class Client
    {
        TCPSocket _sock;
        Arg _args;

        bool _terminated;

      public:
        Client(Socket& client, Arg arg) :
            _sock(client), _args(arg), _terminated(false)
        {
            DEBUG << "New subscriber " << _sock;
        }

        void run()
        {
            string sbuf;
            while(!_terminated)
            {
                char buf[4096];
                int bytes_read = _sock.recv(buf, sizeof(buf));

                if ( bytes_read < 0 ) {
                    ERROR << "recv failed " << strerror(errno);
                    return;
                }
                    
                sbuf.append(buf, bytes_read);

                ISerialString input(sbuf, ISerial::binary);

                int bytes_input = 0;
                while ( true )
                {
                    ptr<StreamEvent> event(new StreamEvent());
                    if ( ! (input >> *event) )
                    {
                        break;
                    }
                    bytes_input = input.pos();

                    // Handle the event received if known how
                    if ( _args )
                    {
                        Status stat = _args(event);
                        if ( ! stat )
                            WARN << "Callback failed with error: "
                                 << stat << "\n on event was: " << event;
                    }
                }

                sbuf.erase(0, bytes_input);
            }
        }

        void terminate()
        {
            _terminated = true;
            ::shutdown(_sock.get_fd(), SHUT_RD);
        }
    };

    typedef Callback< Status, ptr<StreamEvent> > dhcallback;
    typedef ThreadAcceptor<Client<dhcallback>,dhcallback> borealis_acceptor;

    ptr< Threaded<borealis_acceptor> > _listen_thread;
    ptr< Threaded<IOEventLoop> > _loop_thread;

    InetAddress _remote;
    TCPRPCEndpoint _endpoint;

    typedef map<MedusaID, ptr<TCPRPCEndpoint> > Endpoints;
    Endpoints _endpoints;

    // Members for blocked operation.
    ptr<TCPSocket> _send_socket;
    ptr<TCPSocket> _listen_socket;

  public:

    ThreadClient(InetAddress remote)
        : _loop_thread(ptr<Threaded<IOEventLoop> >
                       (new Threaded<IOEventLoop>())),
          _remote(remote), _endpoint(*_loop_thread, TCPSocket(remote))
    {
        *static_cast<Remote<QueryProcessor>*>(this) =
            _endpoint.get<QueryProcessor>("QueryProcessor");

        _endpoints[ MedusaID(to_string(remote)) ] =
            ptr<TCPRPCEndpoint>::unowned(&_endpoint);
    }

    ~ThreadClient() { }

    ptr<TCPRPCEndpoint> endpoint_for(MedusaID remote)
    {
        ptr<TCPRPCEndpoint>& ep = _endpoints[remote];
        if (ep) return ep;

        InetAddress addr(to_string(remote));
        if (!addr)
        {
            WARN << "MedusaID cannot be converted to address: " << remote;
            return ptr<TCPRPCEndpoint>();
        }

        ep.set(new TCPRPCEndpoint(*_loop_thread, TCPSocket(addr)));
        return ep;
    }

    Remote<QueryProcessor> qp_for(MedusaID remote)
    {
        ptr<TCPRPCEndpoint> ep = endpoint_for(remote);
        if (!ep) return Remote<QueryProcessor>();

        return ep->get<QueryProcessor>("QueryProcessor");
    }

    using Remote<QueryProcessor>::add_xml_string;

    using Remote<QueryProcessor>::get_stats;
    using Remote<QueryProcessor>::get_statistics;
    using Remote<QueryProcessor>::get_statistics_name;
    using Remote<QueryProcessor>::get_cpu_statistics;
    using Remote<QueryProcessor>::get_region_statistics;
    using Remote<QueryProcessor>::get_tuple_delivery_statistics;
    using Remote<QueryProcessor>::get_region_cpu_statistics;
    using Remote<QueryProcessor>::get_latency_statistics;

    using Remote<QueryProcessor>::move_boxes;

    using Remote<QueryProcessor>::subscribe_streams;
    using Remote<QueryProcessor>::unsubscribe_stream;


    ////////////////////////////////////////
    //
    // Send helpers.
    //
    Status set_socket(InetAddress dest)
    {
        if ( !_send_socket ||
             (_send_socket && _send_socket->getpeername() != dest) )
        {
            _send_socket = ptr<TCPSocket>(new TCPSocket(dest));
        }

        return _send_socket->stat();
    }

    Status send_event(ptr<StreamEvent> evt)
    {
        if ( _send_socket && _send_socket->stat() )
        {
            OSerialString oss(OSerial::binary|OSerial::nosignature);
            oss << (*evt);

            string buf = oss.str();
            if ( !_send_socket->write(buf.data(), buf.length()) )
            {
                WARN << "Write failed";
                return "Write failed";
            }

            return true;
        }

        return _send_socket->stat();
    }


    ////////////////////////////////////////
    //
    // Receive helpers.
    //
    Status set_listener(InetAddress local, dhcallback cb)
    {
        if ( !_listen_socket ||
             (_listen_socket && _listen_socket->getsockname() != local) )
        {
            _listen_socket =
                ptr<TCPSocket>(new TCPSocket(local, Socket::acceptor));

            Status s = _listen_socket->stat();
            if ( !s ) return s;

            DEBUG << "Created listener...";

            _listen_thread =
                ptr<Threaded<borealis_acceptor> >
                (new Threaded<borealis_acceptor>(_listen_socket, cb));

            _listen_thread->start();
            _listen_thread->detach();
        }

        return true;
    }

    Status set_subscriber(Name stream_name, InetAddress local)
    {
        CatalogSubscription subscription(local, stream_name);

        vector<CatalogSubscription> subs;
        subs.push_back(subscription);
        
        RPC<void> r = subscribe_streams(subs);
        if ( ! r.stat() )
            return "Failed to add TCP subscription: " +
                to_string(r.stat());
    }
    
    IOEventLoop& loop()
    {
        return *_loop_thread;
    }

    void run()
    {
        if ( _loop_thread ) {
            _loop_thread->start();
            _loop_thread->detach();
        }
    }

    void terminate()
    {
        _loop_thread->terminate();

        if ( _listen_thread )
            _listen_thread->terminate();

        TCPSocket dummy(_listen_socket->getsockname());
    }

    string as_string()
    {
        return _remote.as_string();
    }

};

BOREALIS_NAMESPACE_END;

#endif
