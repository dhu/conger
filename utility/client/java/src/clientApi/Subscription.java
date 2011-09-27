package clientApi;

/**
 * The Tuple class implements a tuple that can reflect the tuple data sent from an Aurora output box.
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0
 */
class Subscription {

  /**
   * The name of the output stream to subscribe to.
   */
  String streamName;

  /**
   * The ip address of the StreamEvent receiver.
   */
  String ip;

  /**
   * The port of the StreamEvent receiver.
   */
  int port;

  /**
   * The header definition of a Subscription request.
   */
  static int[] header = {
      0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0xf4, 0x00, 0x00,
      0x00, 0x0e, 0x51, 0x75, 0x65, 0x72, 0x79, 0x50, 0x72, 0x6f, 0x63, 0x65, 
      0x73, 0x73, 0x6f, 0x72, 0x00, 0x00, 0x4e, 0x3b};

  /**
   * Constructs a Subscription.
   * @param streamName the name of the output stream to subscribe to.
   * @param ip the ip address of the StreamEvent receiver.
   * @param port the port of the StreamEvent receiver.
   */
  public Subscription(String streamName, String ip, int port) {
    this.streamName = streamName;
    this.ip = ip;
    this.port = port;
  }

  /**
   * Writes out the header to the CCompatibleOutputStream specified.
   * @param os the CCompatibleOutputStream to write to.
   */
  private void writeHeader(CCompatibleOutputStream os) throws Exception {
    for (int i = 0; i < header.length; i++) {
      os.writeByte( (byte) (header[i]));
    }
  }

  /**
   * Returns the String represention of this Subscription.
   * @return the String represention of this Subscription.
   */
  public String toString() {
    return streamName + " -> " + ip + ":" + port;
  }

  /**
   * Writes this Subscriber out to the CCompatibleOutputStream specified.
   * @param os the CCompatibleOutputStream to write to.
   * @throws java.lang.Exception if there's a problem.
   */
  protected void write(CCompatibleOutputStream os) throws Exception {

    java.io.ByteArrayOutputStream bo = new java.io.ByteArrayOutputStream();
    CCompatibleOutputStream o = new CCompatibleOutputStream(bo);

    o.writeJustBytes("~subscribe_IN={Subscription={Object={Name={S=");
    o.writeJustBytes(streamName);
    o.writeJustBytes("-");
    o.writeJustBytes(ip);
    o.writeJustBytes(":");
    o.writeJustBytes("" + port);
    o.writeJustBytes("-tcp-MyTCPSink;}}Name={S=");
    o.writeJustBytes(streamName);
    o.writeJustBytes(";}StreamDef={Object={Name={S=");
    o.writeJustBytes(streamName);
    o.writeJustBytes(";}}StreamID={Name={S=");
    o.writeJustBytes(streamName);
    if (Marshal.shortRepresentation)
      o.writeJustBytes(";}vector={}}Schema={Object={Name={S=;}}b=0;TupleDescription={ptr={b=0;}}}Partition={MedusaID={S=;}}}S=tcp;MedusaID={S=");
    else
      o.writeJustBytes(";}vector={}}Schema={Object={Name={S=;}}bool=0;TupleDescription={ptr={bool=0;}}}Partition={MedusaID={S=;}}}S=tcp;MedusaID={S=");
    o.writeJustBytes(ip);
    o.writeJustBytes(":");
    o.writeJustBytes("" + port);
    if (Marshal.shortRepresentation)
      o.writeJustBytes(";}S=MyTCPSink;j=2;b=0;S=;}j=1;}");
    else
      o.writeJustBytes(";}S=MyTCPSink;unsigned=2;bool=0;S=;}unsigned=1;}");
    int size = bo.size();
    os.writeInt(size);
    writeHeader(os);
    os.write(bo.toByteArray());
  }

  /**
   * A simple test program.
   */
  public static void main(String[] args) throws Exception {
    Marshal.shortRepresentation = false;
    java.io.FileOutputStream f = new java.io.FileOutputStream("sub.dat");
    CCompatibleOutputStream os = new CCompatibleOutputStream(f);
    String streamName = "borealis://brown.cs.brown.edu/test/output";
    String ip = "127.0.0.1";
    int port = 50000;
    Subscription s = new Subscription(streamName, ip, port);
    s.write(os);
  }
}
