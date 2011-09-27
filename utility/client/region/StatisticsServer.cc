#include <NMSTL/tcprpc>
#include <NMSTL/text>

#include  "StatisticsServer.h"

BOREALIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////////////////////////
//
// Sets the acceptors for the RPC Server
//

void StatisticsServer:: setacceptors()
{
    _bindings.bind( *this, "StatisticsServer" );

    TCPRPCAcceptor tacceptor(*_loop, _port, _bindings);

    if (!tacceptor)
    {   FATAL << "Unable to accept TCP/RPC connections: " << tacceptor.stat();
    }

    NOTICE << "Accepting TCP/RPC connections on " << tacceptor.get_socket().getsockname();


    XMLRPCAcceptor xacceptor(*_loop, _port+XML_RPC_PORT_OFFSET, _bindings);
    if (!xacceptor) 
    {   FATAL << "Unable to accept XML/RPC connections: " << xacceptor.stat();
    }

    NOTICE << "Accepting XML/RPC connections on " << xacceptor.get_socket().getsockname();
}

BOREALIS_NAMESPACE_END

//--- Added by nmstl-rpcgen; do not modify.  Leave at end of file!
#define NMSTL_RPC_DEFINE
#include "rpc_StatisticsServer.h"
#undef NMSTL_RPC_DEFINE
//---
