#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "common.h"
#include <NMSTL/serial>
#include <NMSTL/netioevent>
#include "StreamEvent.h"

BOREALIS_NAMESPACE_BEGIN

class DataHandler;
typedef map< Name, list< pair<MedusaID, ptr<DataHandler> > > > DataHandlers;

class DataHandler : public NetHandler<>
{

 public:

    // Transient errors on this connection
    static string NO_SPACE;
    static string NOT_YET_CONNECTED;

    // Fatal errors on this connection
    static string NO_CONNECTION;
    static string WRITE_FAILED;


    /// A handle on the socket
    IOHandle* _cached_handle;

    /// Is the connection already closed
    bool _terminated;

    /// Is the connection already established
    bool _connected;

    /// Is this a new connection (we never sent data on it yet)
    bool _is_new; 

    /// Method to callback when new StreamEvents arrive
    typedef Callback< Status, ptr<StreamEvent> > dhcallback;


    struct DHArgs
    {
        /// Maximum size of output buffer
        unsigned int max_buffer;
        dhcallback cb;

        DHArgs(unsigned int buff, dhcallback callback = dhcallback())
            : max_buffer(buff), cb(callback) {}

    };
    ptr<DHArgs> _args;

    // Method to callback whenever a connection completes.
    typedef Callback< void, void > dhconnect_callback;
    ptr<dhconnect_callback> _conn_cb;

    DataHandler(IOEventLoop& loop, IOHandle ioh, ptr<DHArgs> args = ptr<DHArgs>(),
                ptr<dhconnect_callback> conn_cb = ptr<dhconnect_callback>()) :
        NetHandler<>(loop,ioh), 
        _cached_handle(&ioh), 
        _terminated(false),
        _connected(false), 
        _is_new(true), 
        _args(args), 
        _conn_cb(conn_cb)
    {
        // If socket is non blocking it is probably not connected at this point
        if ( is_connected() )
            _connected = true;

    }

    virtual ~DataHandler();


    /// Set up a new callback for whenever the next connection completes.
    ///
    Status set_connect_callback(ptr<dhconnect_callback> conn_cb) { _conn_cb = conn_cb; return true; }


    /// Send stream events to remote host
    ///
    Status send_data(ptr<StreamEvent> event);


    bool is_new() { return _is_new; }


    /// Output queue per sender should not exceed the limit above
    ///
    static const uint32  MAX_PER_SENDER_OUTPUT_QUEUE = 40000000; // 40 MB


 protected:

    /// Invoke callback when StreamEvents arrive
    int incoming_data(constbuf buf);


    void connected(Status stat)
    {
        _connected = true;

        if ( _conn_cb )
        {   (*_conn_cb)();
        }
    }


    void end_data(constbuf buf)
    {
        _terminated = true;
        WARN << "Connection closed";
    }
};


BOREALIS_NAMESPACE_END
#endif                        // DATAHANDLER_H
