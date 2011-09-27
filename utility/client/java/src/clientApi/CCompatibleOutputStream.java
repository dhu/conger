package clientApi;

import java.io.*;

/**
 * The CCompatibleOutputStream class provides Linux C compatible output streaming services.
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
public class CCompatibleOutputStream
    extends DataOutputStream {

  /**
   * Constructs a CCompatibleOutputStream that writes to the specified OutputStream.
   * @param os the OutputStream to write to.
   */
  public CCompatibleOutputStream(OutputStream os) {
    super(os);
  }

  /**
   * Writes the specified short value to the OutputStream in the reverse byte order.
   * @param s the short value to write.
   * @throws IOException if a write error occurs.
   */
  public void writeShortReverse(short s) throws IOException {
    s = (short)(( ( (0xff00 & s) >>> 8) |
         ( (0x00ff & s) << 8) ));
    super.writeShort(s);
  }

  /**
   * Writes the specified int value to the OutputStream in the reverse byte order.
   * @param int the int value to write.
   * @throws IOException if a write error occurs.
   */
  public void writeIntReverse(int i) throws IOException {
    i = ( ( (0xff000000 & i) >>> 24) | ( (0x00ff0000 & i) >>> 8) |
         ( (0x0000ff00 & i) << 8) | ( (0x000000ff & i) << 24));
    super.writeInt(i);
  }

  /**
   * Writes the specified long value to the OutputStream in the reverse byte order.
   * @param l the long value to write.
   * @throws IOException if a write error occurs.
   */
  public void writeLongReverse(long l) throws IOException {
    ByteArrayOutputStream b = new ByteArrayOutputStream();
    DataOutputStream os = new DataOutputStream(b);
    os.writeLong(l);
    os.flush();
    byte[] buffer = b.toByteArray();
    for (int i = 0; i < 8; i++) {
      super.writeByte(buffer[7-i]);
    }
  }

  /**
   * Writes the specified String value to the OutputStream in a C-compatible format.
   * @param string the String value to write.
   * @throws IOException if a write error occurs.
   */
  public void writeString(String string) throws IOException {
    int length = string.length();
    super.writeInt(length);
    if (length > 0)
        super.write(string.getBytes());
  }

  /**
   * Writes just the bytes of the specified String value to the OutputStream.
   * @param string the String value to write.
   * @throws IOException if a write error occurs.
   */
  public void writeJustBytes(String string) throws IOException {
    byte[] bytes = string.getBytes();
      super.write(bytes);
  }

}