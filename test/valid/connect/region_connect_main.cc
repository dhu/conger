////////////////////////////////////////////////////////////////////////////////
//
int main( int argc, const char *argv[] )
{
    int            port = DEFAULT_REGION_PORT;
    string         ip   = "localhost";
    RPCBindings    bindings;
//..............................................................................


    server = new RegionServer(port);
    bindings.bind(*server, "RegionServer");

    ip = Util::get_host_address(ip);

    InetAddress listening(ip, port);
    TCPRPCAcceptor acceptor(loop, listening, bindings);

    if (!acceptor)
    {   FATAL << "Unable to accept RPC connections: " << acceptor.stat();
    }

    NOTICE << "Accepting Regional RPC connections on "
           << acceptor.get_socket().getsockname();

#ifdef  CONNECT4
    // Connect to the qp for the downstream node.
    //
    node = Util::form_endpoint("localhost", NODE1);

    client = (ptr<MedusaClient>) new MedusaClient(node);
    query_processor = client->qp_for(MedusaID(node));
#endif

    write_regional_catalog();

    DEBUG  << "run the client event loop...";
    loop.run();

    NOTICE << "terminating..";

    return(0);
}
