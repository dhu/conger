package clientApi;

import java.lang.reflect.*;
import java.io.*;

/**
 * The CompactSerializable class is a super class for all Linux C-compatible, compact-serializable Java objects.
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
abstract class CompactSerializable {

  /**
   * Constructs an empty CompactSerializable.
   */
  public CompactSerializable() {
  }

  /**
   * Returns the size of this CompactSerializable in bytes.
   * @return the size of this CompactSerializable in bytes.
   */
  public int getSize() throws Exception {
    int size = 0;
    java.util.Vector fieldVector = getFieldVector();
    for (int i = 0; i < fieldVector.size(); i++) {
      Field field = (Field) (fieldVector.elementAt(i));
      Class type = field.getType();
      if (type == Byte.TYPE) {
        size += 1;
      }
      else if (type == Short.TYPE) {
        size += 2;
      }
      else if (type == Integer.TYPE) {
        size += 4;
      }
      else if (type == Long.TYPE) {
        size += 8;
      }
      else if (type == Double.TYPE) {
        size += 8;
      }
      else if (type.getName().equals("[B")) {
        size += ( (byte[]) (field.get(this))).length;
      }
    }
    return size;
  }

  /**
   * Writes this ComactSerializable to the CCompatibleOutputStream specified.
   * @param os the CCompatibleOutputStream to write to.
   * @throws Exception if a write error occurs.
   */
  protected void write(CCompatibleOutputStream os) throws Exception {
    java.util.Vector fieldVector = getFieldVector();
    for (int i = 0; i < fieldVector.size(); i++) {
      Field field = (Field) (fieldVector.elementAt(i));
      Class type = field.getType();
      if (type == Byte.TYPE) {
        os.writeByte(field.getByte(this));
      }
      else if (type == Short.TYPE) {
        os.writeShortReverse(field.getShort(this));
      }
      else if (type == Integer.TYPE) {
        os.writeIntReverse(field.getInt(this));
      }
      else if (type == Long.TYPE) {
        os.writeLongReverse(field.getLong(this));
      }
      else if (type == Double.TYPE) {
        os.writeDouble(field.getDouble(this));
      }
      else if (type.getName().equals("[B")) {
        os.write( (byte[]) (field.get(this)));
      }
    }
  }

  /**
   * Reads this ComactSerializable from the CCompatibleInputStream specified.
   * @param is the CCompatibleInputStream to read from.
   * @throws Exception if a read error occurs.
   */
  protected void read(CCompatibleInputStream is) throws Exception {
    java.util.Vector fieldVector = getFieldVector();
    for (int i = 0; i < fieldVector.size(); i++) {
      Field field = (Field) (fieldVector.elementAt(i));
      Class type = field.getType();
      if (type == Byte.TYPE) {
        field.setByte(this, is.readByte());
      }
      else if (type == Short.TYPE) {
        field.setShort(this, is.readShortReverse());
      }
      else if (type == Integer.TYPE) {
        field.setInt(this, is.readIntReverse());
      }
      else if (type == Long.TYPE) {
        field.setLong(this, is.readLongReverse());
      }
      else if (type == Double.TYPE) {
        field.setDouble(this, is.readDouble());
      }
      else if (type.getName().equals("[B")) {
        is.read( (byte[]) (field.get(this)));
      }
    }
  }

  /**
   * Returns a byte-array representation of this CompactSerializable.
   * @return a byte-array representation of this CompactSerializable.
   * @throws Exception if a write error occurs.
   */
  public byte[] toByteArray() throws Exception {
    int size = getSize();
    if (size == 0) {
      return null;
    }
    ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
    CCompatibleOutputStream os = new CCompatibleOutputStream(byteStream);
    write(os);
    return byteStream.toByteArray();
  }

  /**
   * Reads from a byte array.
   * @param buffer the byte array to read from.
   * @throws Exception if a read error occurs.
   */
  public void read(byte[] buffer) throws Exception {
    read(buffer, 0);
  }

  /**
   * Reads from a byte array.
   * @param buffer the byte array to read from.
   * @param offset the offset to start reading.
   * @throws Exception if a read error occurs.
   */
  public void read(byte[] buffer, int offset) throws Exception {
    read(new CCompatibleInputStream(new ByteArrayInputStream(buffer, offset,
        buffer.length - offset)));
  }

  /**
   * Returns a vector of the fields this ComactSerializable has.
   * @return a vector of the fields this ComactSerializable has.
   * @throws Exception if a problem happens.
   */
  protected java.util.Vector getFieldVector() throws Exception {
    java.util.Vector fieldVector = new java.util.Vector();
    Class c = getClass();
    while (c != Class.forName("java.lang.Object")) {
      Field[] fields = c.getDeclaredFields();
      for (int i = fields.length - 1; i >= 0; i--) {
        fieldVector.add(0, fields[i]);
      }
      c = c.getSuperclass();
    }
    return fieldVector;
  }

}