package medusaXmlRpc;

public class Args {

    public static String DEFAULT_SERVER_IP = "127.0.0.1";
    public static String DEFAULT_SERVER_PORT = "15000";
    public static String DEFAULT_APP_IP = "127.0.0.1";
    public static String DEFAULT_APP_PORT = "15020";

    public String serverIP, serverPort, appIP, appPort;
    public String appAddr() { 
        return appIP+":"+appPort;
    }

    public String serverAddr() {
        return ""+ serverIP + ":" + serverPort;
    }
    
    public String serverRPC() {
        return "http://" + serverIP + ":" + (Integer.parseInt(serverPort)+1) 
            + "/RPC2";
    }

    public void setDefaults() {
        serverIP = DEFAULT_SERVER_IP;
        serverPort = DEFAULT_SERVER_PORT; 
        appIP = DEFAULT_APP_IP; 
        appPort = DEFAULT_APP_PORT;
    }

    public Args(String sIP, String sPort, String aIP, String aPort) {
        serverIP = sIP; 
        serverPort = sPort; 
        appIP = aIP; 
        appPort = aPort;
    }

    public static String usage() {
        StringBuffer buff = new StringBuffer();
        buff.append("  [-d medusaIP:medusaPort] [-a appIP:appPort]\n");
        buff.append("  * medusaIP:medusaPort: IP:port of Medusa node to which app should connect\n");
        buff.append("  * default: " + DEFAULT_SERVER_IP + ":" + DEFAULT_SERVER_PORT + "\n");
        buff.append("  * appIP:appPort: IP:port that app should use to receive events from Medusa\n");
        buff.append("  * default: " + DEFAULT_APP_IP + ":" + DEFAULT_APP_PORT + "\n");
        return buff.toString();
    }

    public Args(String[] args) {

        setDefaults();
        int i = 0;
        while (i < args.length) {
            String arg = args[i++];
                        
            if (arg.equals("-d")) {
                String[] serverInfo = args[i++].split(":");
                serverIP = serverInfo[0];
                if ( serverInfo.length > 1)
                    serverPort = serverInfo[1];                
            }

            if (arg.equals("-a")) {
                String[] appInfo = args[i++].split(":");
                appIP = appInfo[0];
                if ( appInfo.length > 1)
                    appPort = appInfo[1];                
            }

        }
        System.out.println("\nApplication will connect to Medusa at addr: " + serverIP + ":" + serverPort);
        System.out.println("Application will listen on: " + appIP + ":" + appPort);
    }

}
