public class QueryGen {
	
	static String routingServer = "128.148.33.121";
	
	static String routingPort = "20000";

	static String router = routingServer+":"+routingPort;

	static java.util.Vector servers = new java.util.Vector();

	static java.util.Vector ports = new java.util.Vector();

	static int number_of_units = 8 * 2;

	static int number_of_agg1_HA1 = 2;

	static int number_of_agg2_HA1 = 0;

	static int number_of_agg1_HA2 = 0;

	static int number_of_agg2_HA2 = 2;

	static String query_header = "<?xml version=\"1.0\"?>\n"
			+ "<!DOCTYPE borealis SYSTEM \"../../src/src/borealis.dtd\">\n"
			+ "<!-- Borealis query diagram for:  ha_demo.cc -->\n"
			+ "<borealis>\n" + "\n"
			+ "    <input  stream=\"input1\" schema=\"PacketSchema\" />\n"
			+ "    <input  stream=\"input2\" schema=\"PacketSchema\" />\n"
			+ "    <output stream=\"output\" schema=\"CountSchema\"/>\n" + "\n"
			+ "    <schema name=\"PacketSchema\">\n"
			+ "        <field name=\"timestamp\" type=\"int\"/>\n"
			+ "        <field name=\"source\" type=\"int\"/>\n"
			+ "    </schema>\n" + "\n" + "    <schema name=\"CountSchema\">\n"
			+ "        <field name=\"source\" type=\"int\"/>\n"
			+ "        <field name=\"timestamp\" type=\"int\"/>\n"
			+ "        <field name=\"agg_id\" type=\"int\"/>\n"
			+ "        <field name=\"count\" type=\"int\"/>\n"
			+ "    </schema>\n" + "\n";

	static String get_query_u() {
		String query_u = "    <query name=\"QueryU\">\n";
		for (int i = 1; i <= number_of_units / 2; i++) {
			query_u += "        <box name=\"union.u" + (100 + i)
					+ "\" type=\"union\">\n"
					+ "            <in  stream=\"input1\"/>\n"
					+ "            <out stream=\"" + (100 + i) + "\"/>\n"
					+ "        </box>\n";
			query_u += "        <box name=\"union.u" + (200 + i)
					+ "\" type=\"union\">\n"
					+ "            <in  stream=\"input2\"/>\n"
					+ "            <out stream=\"" + (200 + i) + "\"/>\n"
					+ "        </box>\n";
		}
		query_u += "    </query>\n" + "\n";
		return query_u;
	};

	static String get_aggregate(int id, int input) {
		String aggregate = "        <box name=\""
				+ id
				+ "\" type=\"aggregate\">\n"
				+ "            <in  stream=\""
				+ input
				+ "\"/>\n"
				+ "            <out stream=\""
				+ id
				+ "\"/>\n"
				+ "            <parameter   name=\"advance\"               value=\"1\"        />\n"
				+ "            <parameter   name=\"aggregate-function.0\"  value=\"avg("
				+ id
				+ ")\"  />\n"
				+ "            <parameter   name=\"aggregate-function-output-name.0\"  value=\"agg_id\"  />\n"
				+ "            <parameter   name=\"aggregate-function.1\"  value=\"count()\"  />\n"
				+ "            <parameter   name=\"aggregate-function-output-name.1\"  value=\"count\"  />\n"
				+ "            <parameter   name=\"order-by\"              value=\"FIELD\"    />\n"
				+ "            <parameter   name=\"order-on-field\"        value=\"timestamp\"    />\n"
				+ "            <parameter   name=\"group-by\"              value=\"source\"    />\n"
				+ "            <parameter   name=\"window-size\"           value=\"10\"        />\n"
				+ "            <parameter   name=\"window-size-by\"        value=\"VALUES\"   />\n"
				+ "            <parameter   name=\"drop-empty-outputs\"    value=\"1\"   />\n"
				+ "        </box>\n";
		return aggregate;
	}

	static String get_queries() {

		String queries = "";

		for (int i = 1; i <= servers.size(); i++) {
			String query = "     <query name=\"Query" + i + "\">\n";
			for (int j = 1; j <= number_of_units / 2; j++) {
				for (int k = 1; k <= number_of_agg1_HA1; k++) {
					int input = 100 + j;
					int id = i * 10000 + 1 * 1000 + j * 10 + k;
					query += get_aggregate(id, input);
				}
			}
			for (int j = 1; j <= number_of_units / 2; j++) {
				for (int k = 1; k <= number_of_agg2_HA2; k++) {
					int input = 200 + j;
					int id = i * 10000 + 2 * 1000 + j * 10 + k;
					query += get_aggregate(id, input);
				}
			}
			query += "     </query>\n" + "\n";
			queries += query;
		}
		return queries;
	};

	static String get_query_d() {
		String query_d = "    <query name=\"QueryD\">\n"
				+ "        <box name=\"union.output\" type=\"union\">\n";
		for (int i = 1; i <= servers.size(); i++)
			query_d += "            <in  stream=\"" + i + "" + (201)
					+ "1\"/>\n";
		query_d += "            <out stream=\"output\"/>\n"
				+ "        </box>\n" + "    </query>\n" + "\n";

		return query_d;
	}

	static String get_deployment() {
		String deployment = "<?xml version=\"1.0\"?>\n"
				+ "<!DOCTYPE borealis SYSTEM \"../../src/src/borealis.dtd\">\n"
				+ "\n" + "<!-- Borealis query diagram for:  ha_demo.cc -->\n"
				+ "\n" + "<deploy>\n"
				+ "    <publish   stream=\"input1\"  node=\"" + router
				+ "\"/>\n" + "    <publish   stream=\"input2\"  node=\""
				+ router + "\"/>\n"
				+ "    <subscribe stream=\"output\"  node=\"" + router
				+ "\"/>\n" + "\n" + "    <node   endpoint=\"" + router
				+ "\" query=\"QueryU\" />\n";
		for (int i = 1; i <= servers.size(); i++) {
			deployment += "    <node   endpoint=\"" + servers.elementAt(i - 1) + ":"+ports.elementAt(i-1)
					+ "\" query=\"Query" + i + "\" />\n";
		}

		deployment += "    <node   endpoint=\"" + router
				+ "\" query=\"QueryD\" />\n" + "\n" + "</deploy>\n";
		return deployment;

	}

	static String get_script() {
		String script = "#!/bin/sh\n"
				+ "\n"
				+ "if [ $# -lt 1 ]\n"
				+ "then\n"
				+ "duration=100\n"
				+ "else\n"
				+ "duration=$1\n"
				+ "fi\n"
				+ "\n"
				+ "ARGS=\"$2 $3 $4 $5 $6 $7 $8\"\n"
				+ "echo \"==============================\"\n"
				+ "echo \"test case name: $1\"\n"
				+ "echo \"duration: $duration\"\n"
				+ "echo \"args: $ARGS\"\n"
				+ "\n"
				+ "\n"
				+ "CVS_SANDBOX=${CVS_SANDBOX:-\"${HOME}\"}\n"
				+ "BOREALIS_HOME=${CVS_SANDBOX}/borealis\n"
				+ "BOREALIS_SRC_HOME=${BOREALIS_HOME}/src/src\n"
				+ "CLIENT_HOME=${BOREALIS_HOME}/demo/ha\n"
				+ "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_NMSTL}/lib:${INSTALL_ANTLR}/lib\n"
				+ "PATH=${PATH}:${BOREALIS_SRC_HOME}:${BOREALIS_HOME}/tool/marshal:${BOREALIS_HOME}/tool/head\n"
				+ "\n" + "ROUTER=\""+routingServer+"\"   #yacht\n\n";
		
		for (int i = 0; i < servers.size(); i++) {
			String server = servers.elementAt(i).toString();
			script += "SERVER"+(i+1) + "=\""+ server + "\"\n";
		}
		script += "\n";
		script += "rm ~/*.log -f\n";
		script += "killall xterm;killall lt-borealis\n";
		for (int i = 0; i < servers.size(); i++) {
			script += "sleep 2;ssh ${SERVER"+(i+1)+"} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis\n";
		}
		script += "\n";
		script += "sleep 3;echo \"router\";xterm -T router -geometry 80x5+100+100 -e sh -l -c \"LD_LIBRARY_PATH=$LD_LIBRARY_PATH ${BOREALIS_SRC_HOME}/borealis -d $ROUTER:20000 2>&1 |tee router.log\" &\n";
		script += "\n";
		for (int i = 1; i <= servers.size(); i++) {
			String serverArgs = "S"+i+"ARGS=\"-d $SERVER"+i+":"+ports.elementAt(i-1);
			for (int j = 0; j < servers.size()-1; j++) {
					serverArgs += (" -p $SERVER"+((j+i)%servers.size()+1)+":"+ports.elementAt((j+i)%servers.size()));
			}
			serverArgs += " $ARGS\"\n";
			serverArgs += "sleep 1;echo \"borealis"+i+"\";xterm -T borealis"+i+" -geometry 80x5+500+"+i+"00 ";
			serverArgs += "-e env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} ";
			serverArgs += "S"+i+"ARGS=\"$S"+i+"ARGS\" SERVER"+i+"=\"$SERVER"+i;
			serverArgs += "\" sh -c 'ssh $SERVER"+i+" env LD_LIBRARY_PATH=$LD_LIBRARY_PATH BOREALIS_LOGGING=1 BOREALIS_SRC_HOME=${BOREALIS_SRC_HOME} ";
			serverArgs += "S"+i+"ARGS=\"'${S"+i+"ARG}S'\" ${BOREALIS_SRC_HOME}/borealis $S"+i+"ARGS 2>&1 |tee borealis"+i+".log' &";
			script += serverArgs + "\n\n";
		}
		
		script += "sleep 13;echo \"ha_demo\";xterm -T ha_demo -geometry 80x5+500+700 -e sh -l -c -c \"LD_LIBRARY_PATH=$LD_LIBRARY_PATH PATH=$PATH ./ha_demo 2>&1 |tee ha_demo.log\" &\n";
		script += "\n";
		script += "sleep $duration";
		script += "\n";
		script += "killall xterm;killall lt-borealis\n";
		for (int i = 0; i < servers.size(); i++) {
			script += "sleep 2;ssh ${SERVER"+(i+1)+"} env LD_LIBRARY_PATH=$LD_LIBRARY_PATH killall lt-borealis\n";
		}
		script += "\n";
		script += "sleep 3;mv ~/*.log .;./rename.sh log \"$1.dat\"\n";
		for (int i = 1; i <= servers.size(); i++) {
			script += "./rename.sh $SERVER"+i+":"+ports.elementAt(i-1)+" server"+i+"\n";
		}
		return script;
	}

	public static void main(String[] args) throws Exception {

		System.out.println(args.length);

		number_of_units = Integer.parseInt(args[0]) * 2;
		number_of_agg1_HA1 = Integer.parseInt(args[1]);
		number_of_agg2_HA1 = 0;
		number_of_agg1_HA2 = 0;
		number_of_agg2_HA2 = Integer.parseInt(args[2]);

		for (int i = 3; i < args.length; i++) {
			String s = args[i];
			servers.add(s.substring(0, s.indexOf(':')));
			ports.add(s.substring(s.indexOf(':')+1));
			System.out.println(args[i]);
		}
		try {
			java.io.FileOutputStream out = new java.io.FileOutputStream(
					"ha_demo.xml");
			String query = query_header + get_query_u() + get_queries()
					+ get_query_d() + "</borealis>\n";
			out.write(query.getBytes());
			System.out.println(query);
			out.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			java.io.FileOutputStream out = new java.io.FileOutputStream(
					"ha_demo_deployment.xml");
			String deployment = get_deployment();
			out.write(deployment.getBytes());
			System.out.println(deployment);
			out.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			java.io.FileOutputStream out = new java.io.FileOutputStream(
					"inner_run.sh");
			String script = get_script();
			out.write(script.getBytes());
			System.out.println(script);
			out.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}