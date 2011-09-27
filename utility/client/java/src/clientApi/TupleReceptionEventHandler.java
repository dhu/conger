package clientApi;

/**
 * The handler for the event of receiving a Tuple.
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
abstract public class TupleReceptionEventHandler {

  /**
   * Is invoked when the event of receiving a Tuple occurs.
   * @param output the borealis output stream from which the Tuple came.
   * @param tuple the Tuple received.
   */
  abstract public void handle(String output, Tuple tuple);

}