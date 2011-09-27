package clientApi;

/**
 * The StreamEvent class is a Java-counterpart of the C++ StreamEvent class.
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
class StreamEvent {

  /**
   * Registers an input with its associated tuple class.
   * @param input the name of the Borealis input.
   * @param tupleClass the name of the tuple class.
   */
  public static void register(String input, String tupleClass) throws Exception {
    Class c = Class.forName(tupleClass);
    Marshal.input2class.put(input, c);
  }

  /**
   * Returns the class object associated with the input specified.
   * @param input the name of the Borealis input.
   * @return the class object associated with the input specified.
   */
  static Class getClass(String input) throws Exception {
    Class c = (Class) (Marshal.input2class.get(input));
    if (c == null) {
      throw new Exception("now a registered input");
    }
    return c;
  }

  /**
   * The name of the stream.
   */
  String streamName;

  /**
   * The stream identifier.
   */
  String streamIdentifier;
  
  /**
   * The number of connection points.  
   */
   int cpCount = 0;

  /**
   * The connection points.
   */
   java.util.Vector cPoints = new java.util.Vector();

  /**
   * The inet address.
   */
  String inetAddress;

  /**
   * A flag representing if the connection is closed.
   */
  byte closed;

  /**
   * A flag representing if it is to be injected.
   */
  byte injected;

  /**
   * The number of tuples inserted.
   */
  int insertCount = 0;

  /**
   * The size of the tuples inserted.
   */
  int tupleSize;

  /**
   * The tuples inserted.
   */
  java.util.Vector tuples = new java.util.Vector();

  /**
   * Constructs a StreamEvent.
   */
  public StreamEvent() {
    this("");

  }

  /**
   * Constructs a StreamEvent.
   * @param streamName the name of the stream associated with this StreamEvent.
   */
  public StreamEvent(String streamName) {
    this.streamName = streamName;
    this.streamIdentifier = streamName;
    this.closed = 0;
    this.injected = 0;
    this.tupleSize = 0;
  }

  /**
   * Adds a tuple to this StreamEvent.
   * @param tuple the tuple to add.
   */
  public void add(Tuple tuple) throws Exception {
    tupleSize = tuple.getSize();
    insertCount++;
    tuples.add(tuple);
  }

  /**
   * Writes this StreamEvent to the CCompatibleOutputStream specified.
   * @param os the CCompatibleOutputStream to write to.
   * @throws if an problem occurs.
   */
  protected void write(CCompatibleOutputStream os) throws Exception {
    os.writeString(streamName);
    os.writeString(streamIdentifier);
    os.writeInt(cpCount);
    for (int i = 0; i < cpCount; i++) {
      os.writeString( (String) (cPoints.elementAt(i)));
    }
    os.writeString(inetAddress);
    os.writeByte(closed);
    os.writeByte(injected);
    os.writeInt(insertCount);
    os.writeInt(0);
    os.writeInt(tupleSize);
    for (int i = 0; i < insertCount; i++) {
      ( (Tuple) (tuples.elementAt(i))).write(os);
    }
  }

  /**
   * Reads this StreamEvent from the CCompatibleInputStream specified.
   * @param is the CCompatibleInputStream to read from.
   * @throws if an problem occurs.
   */
  protected void read(CCompatibleInputStream is) throws Exception {
    streamName = is.readString();
    streamIdentifier = is.readString();
    cpCount = is.readInt();
    for (int i = 0; i < cpCount; i++) {
      cPoints.add(is.readString());
    }
    inetAddress = is.readString();
    closed = is.readByte();
    injected = is.readByte();
    insertCount = is.readInt();
    tupleSize = is.readInt();
    tupleSize = is.readInt();
    Class type = (Class) (Marshal.input2class.get(streamName));
    if (type != null) {
      for (int i = 0; i < insertCount; i++) {
        Tuple tuple = (Tuple) (type.newInstance());
        tuple.read(is);
        tuples.add(tuple);
      }
    }
  }

  /**
   * Returns the String representation of this StreamEvent object.
   * @return the String representation of this StreamEvent object.
   */
  public String toString() {
    String s = "{";
    s += inetAddress;
    s += ", ";
    int nTuples = tuples.size();
    for (int i = 0; i < nTuples; i++) {
      s += tuples.elementAt(i);
    }
    return s + "}";
  }

  /**
   * A simple test program.
   * @param args the arguments.
   * @throws java.lang.Exception if there's a problem.
   */
  public static void main(String[] args) throws Exception {
    String VIDEOOUTPUT = "borealis://brown.cs.edu/mitre_demo/videooutput";
    StreamEvent.register(VIDEOOUTPUT, "ImageData");
    CCompatibleInputStream c = new CCompatibleInputStream(new java.io.
        FileInputStream("a0.dat"));
    StreamEvent e = new StreamEvent();
    e.read(c);
    System.out.println(e);
    e = new StreamEvent();
    e.read(c);
    System.out.println(e);
  }

}