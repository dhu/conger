package clientApi;

import java.lang.reflect.*;
import java.io.*;

/**
 * The TupleHeader class defines the tuple header to be used in Borealis.
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
class TupleHeader
    extends CompactSerializable {

  /**
   * The timestamp as of system entrance (part 1). The first node assigns it.
   */
  public int tv1 = 0;

  /**
   * The timestamp as of system entrance (part 2). The first node assigns it.
   */
  public int tv2 = 0;

  /**
   * The timestamp as of node entrance (part 1). Each node assigns it.
   */
  public int local_tv1 = 0;

  /**
   * The timestamp as of node entrance (part 2). Each node assigns it.
   */
  public int local_tv2 = 0;

  /**
   * The tuple id.
   */
  public int tuple_id = 1;

  /**
   * The tuple type.
   */
  public int tuple_type = 0; //  tuple_type = TupleType::INSERTION;

  /**
   * The revision id.
   */
  public int rev_id = -1;

  /**
   * The QoS field.
   */
  public double quality = 0;

  /**
   * The window specification.
   */
  public int win_spec = -1;

  /**
   * The fake flag.
   */
  public byte fake_flag = 0;

  /**
   * The rev field.
   */
  public int rev_field = 0;

  /**
   * The timestamp value for Magda: part 1.
   */
  public int m_tv1 = 0;

  /**
   * The timestamp value for Magda: part 2.
   */
  public int m_tv2 = 0;

}