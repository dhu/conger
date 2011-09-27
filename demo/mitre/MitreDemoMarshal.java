import clientApi.*;

/**
 * The marshaling code for the mitre demo (should be generated automatically).
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
abstract class MitreDemoMarshal extends Marshal {

  static String ELEMENTINPUT = "borealis://brown.cs.edu/mitre_demo/elementinput";
  static String ELEMENTINPUT_NODE = "localhost";
  static int ELEMENTINPUT_PORT = 15000;

  static String VIDEOINPUT = "borealis://brown.cs.edu/mitre_demo/videoinput";
  static String VIDEOINPUT_NODE = "localhost";
  static int VIDEOINPUT_PORT = 15000;

  static String ACROSSTHELINEOUTPUT =
      "acrossthelineoutput";
  static String ACROSSTHELINEOUTPUT_NODE = "localhost:15000";

  static String ANALYSISOUTPUT =
      "analysisoutput";
  static String ANALYSISOUTPUT_NODE = "localhost:15000";

  static String ENEMYAIRCRAFTOUTPUT =
      "enemyaircraftoutput";
  static String ENEMYAIRCRAFTOUTPUT_NODE = "localhost:15000";

  static String ENEMYTANKSOUTPUT =
      "enemytanksoutput";
  static String ENEMYTANKSOUTPUT_NODE = "localhost:15000";

  static String VIDEOOUTPUT = "videooutput";
  static String VIDEOOUTPUT_NODE = "localhost:15000";

  /**
   * Constructs a MitreDemoMarshal object.
   * @param receivePort my tcp port to receive StreamEvents.
   * @param receivePort my tcp port to receive StreamEvents.
   * @param handler the tuple reception event handler.
   */
  MitreDemoMarshal(int receivePort, TupleReceptionEventHandler handler) throws Exception {
    super(receivePort, handler);
    register(ACROSSTHELINEOUTPUT, "AppElement");
    register(ANALYSISOUTPUT, "AppElement");
    register(ENEMYAIRCRAFTOUTPUT, "AppElement");
    register(ENEMYTANKSOUTPUT, "AppElement");
    register(VIDEOOUTPUT, "ImageData");
  }
}
