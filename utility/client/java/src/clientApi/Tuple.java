package clientApi;

import java.lang.reflect.*;
import java.io.*;

/**
 * The Tuple is the super class for all the tuples to be used in Borealis.
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
public abstract class Tuple extends CompactSerializable {

  /**
   * The header of this Tuple.
   */
  TupleHeader header = new TupleHeader();

  /**
   * Returns the header of this Tuple.
   * @return the header of this Tuple.
   */
  TupleHeader getHeader() {
    return header;
  }

  /**
   * Returns the size of this Tuple in bytes.
   * @return the size of this Tuple in bytes.
   * @throws Exception if a problem occurs during reflection.
   */
  public int getSize() throws Exception {
    return header.getSize()+super.getSize();
  }

  /**
   * Returns the size of this Tuple without header in bytes.
   * @return the size of this Tuple without header in bytes.
   * @throws Exception if a problem occurs during reflection.
   */
  public int getSizeWithoutHeader() throws Exception {
    return super.getSize();
  }

  /**
    * Writes this Tuple to the CCompatibleOutputStream specified.
    * @param os the CCompatibleOutputStream to write to.
    * @throws Exception if a write error occurs.
    */
   public void write(CCompatibleOutputStream os) throws Exception {
     header.write(os);
     super.write(os);
   }

   /**
     * Writes this Tuple without header to the CCompatibleOutputStream specified.
     * @param os the CCompatibleOutputStream to write to.
     * @throws Exception if a write error occurs.
     */
    public void writeWithoutHeader(CCompatibleOutputStream os) throws Exception {
      super.write(os);
    }

    /**
     * Reads this Tuple from the CCompatibleInputStream specified.
     * @param is the CCompatibleInputStream to read from.
     * @throws Exception if a read error occurs.
     */
    public void read(CCompatibleInputStream is) throws Exception {
      header.read(is);
      super.read(is);
    }

    /**
     * Reads this Tuple without header from the CCompatibleInputStream specified.
     * @param is the CCompatibleInputStream to read from.
     * @throws Exception if a read error occurs.
     */
    public void readWithoutHeader(CCompatibleInputStream is) throws Exception {
      super.read(is);
    }

}