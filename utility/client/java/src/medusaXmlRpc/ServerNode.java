package medusaXmlRpc;

/**
 * Models one server node with an IP and port
 * Provides utility methods to get the XML-RPC endpoint
 * or network address for that node
 */
public class ServerNode {

    public String serverIP, serverPort;

    /**
     * @return the IP:port address of this server as a string
     */
    public String serverAddr() {
        return ""+ serverIP + ":" + serverPort;
    }
    
    /**
     * @return the address of the server for XML-RPC connections
     */ 
    public String serverRPC() {
        return "http://" + serverIP + ":" + (Integer.parseInt(serverPort)+1) 
            + "/RPC2";
    }

    /**
     * Constructs a new ServerNode from and IP and port specified
     * as Strings
     */
    public ServerNode(String sIP, String sPort) {
        serverIP = sIP; 
        serverPort = sPort; 
    }

    /**
     * Constructs a new ServerNode from a string that encodes
     * the IP and port of the server in the form "IP:port"
     */
    public ServerNode(String fromString) {

        String[] serverInfo = fromString.split(":");
        serverIP = serverInfo[0];
        if ( serverInfo.length > 1)
            serverPort = serverInfo[1];
        else throw new IllegalArgumentException("Server can only be constructed from an IP:port pair");
    }

    public String toString() {
        return "ServerNode-" + serverAddr();
    }
}
