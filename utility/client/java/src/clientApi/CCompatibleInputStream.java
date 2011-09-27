package clientApi;

import java.io.*;

/**
 * The CCompatibleInputStream class provides Linux C compatible input streaming services.
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
public class CCompatibleInputStream
    extends DataInputStream {

  /**
   * Constructs a CCompatibleInputStream that reads from the specified InputStream.
   * @param is the InputStream to read from.
   */
  public CCompatibleInputStream(InputStream is) {
    super(is);
  }

  /**
   * Reads the specified short value from the InputStream in the reverse byte order.
   * @return the short value read.
   * @throws IOException if a read error occurs.
   */
  public short readShortReverse() throws IOException {
    short s = super.readShort();
    s = (short)(( ( (0xff00 & s) >>> 8) | ( (0x00ff & s) << 8) ));
    return s;
  }

  /**
   * Reads the specified int value from the InputStream in the reverse byte order.
   * @return the int value read.
   * @throws IOException if a read error occurs.
   */
  public int readIntReverse() throws IOException {
    int i = super.readInt();
    i = ( ( (0xff000000 & i) >>> 24) | ( (0x00ff0000 & i) >>> 8) |
         ( (0x0000ff00 & i) << 8) | ( (0x000000ff & i) << 24));
    return i;
  }

  /**
   * Reads the specified long value from the InputStream in the reverse byte order.
   * @return the long value read.
   * @throws IOException if a read error occurs.
   */
  public long readLongReverse() throws IOException {
    byte[] buffer = new byte[8];
    for (int i = 0; i < 8; i++) {
      buffer[7-i] = super.readByte();
    }
    return new DataInputStream(new ByteArrayInputStream(buffer)).readLong();
  }

  /**
   * Reads the specified String value from the InputStream in the C-Compatible format.
   * @return the String value read.
   * @throws IOException if a read error occurs.
   */
  public String readString() throws IOException {
    int length = readInt();
    if (length == 0)
        return "";
    byte[] buffer = new byte[length];
    read(buffer);
    return new String(buffer);
  }

}