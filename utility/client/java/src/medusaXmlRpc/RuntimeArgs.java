package medusaXmlRpc;

public class RuntimeArgs {
        
    public boolean noAurora = false;
    public boolean noDef = false;
    public boolean fakeData = false;
    public Args network;
    
    public static void usage() {
        System.out.println("\nUsage: java motessensors.FireApp [ip-port-options] [runtime-options]");
        System.out.println("-> ip-port-options:");
        System.out.println(Args.usage());
        System.out.println("-> other-options:");
        System.out.println("  [-noAurora] [-noDef] [-fakeData]");
        System.out.println("  * noAurora: run the query using only Medusa operators");
        System.out.println("  * noDef: skip definitions of schemas/streams/queries");
        System.out.println("  * fakeData: generate test data instead of sending real data");
        System.exit(0);
    }

    public RuntimeArgs(String[] args) {

        network = new Args(args);

        int i = 0;
        while (i < args.length) {            
            String arg = args[i++];

            if ( arg.equals("-h") || arg.equals("-?") )
                usage();

            if (arg.equals("-noAurora"))
                noAurora = true;
            if (arg.equals("-fakeData"))
                fakeData = true;
            if (arg.equals("-noDef"))
                noDef = true;
        }
    }

}
