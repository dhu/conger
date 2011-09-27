import clientApi.*;

/**
 * The marshaling code for the mitre demo (should be generated automatically).
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
class MitreDemoMarshalImpl
    extends MitreDemoMarshal {

  static String DEFAULT_IP = "127.0.0.1";
  static int DEFAULT_RPC_PORT = 15000;
  static int DEFAULT_RECEIVE_PORT = 35000;

  /**
   * Constructs a MitreDemoMarshalImpl object.
   * @param receivePort my tcp port to receive StreamEvents.
   * @param handler the tuple reception handler.
   */
  MitreDemoMarshalImpl(int receivePort,
                       TupleReceptionEventHandler handler) throws
      Exception {
    super(receivePort, handler);

    connectOutput(ACROSSTHELINEOUTPUT, ACROSSTHELINEOUTPUT_NODE);
    connectOutput(ANALYSISOUTPUT, ANALYSISOUTPUT_NODE);
    connectOutput(ENEMYAIRCRAFTOUTPUT, ENEMYAIRCRAFTOUTPUT_NODE);
    connectOutput(ENEMYTANKSOUTPUT, ENEMYTANKSOUTPUT_NODE);
    connectOutput(VIDEOOUTPUT, VIDEOOUTPUT_NODE);
  }

  /**
   * A simple test program.
   */
  public static void main(String[] args) throws Exception {

    if (args.length > 0) {
      DEFAULT_IP = args[0];
    }
    System.out.println(DEFAULT_IP);

    MitreDemoMarshalImpl marshal = new MitreDemoMarshalImpl(MitreDemoMarshalImpl.DEFAULT_RECEIVE_PORT,
        new TestTupleReceptionEventHandler());

  }
}

class TestTupleReceptionEventHandler
    extends TupleReceptionEventHandler {

  public TestTupleReceptionEventHandler() {
  }

  public void handle(String output, Tuple tuple) {
    System.out.println(tuple);
  }
}

