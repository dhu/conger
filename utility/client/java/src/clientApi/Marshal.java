package clientApi;

/**
 * The Java-counterpart of the marshaling code.
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
abstract public class Marshal {

  /**
   * Whether to use the short representation or not. If gcc 3.3.5 is used, then this static field must be set to false.
   */
  public static boolean shortRepresentation = false;

  /**
   * Input-to-class has map.
   */
  static java.util.HashMap input2class = new java.util.HashMap();

  /**
   * The default offset for data ports.
   */
  public static int DATA_PORT_OFFSET = 2;

  /**
   * My tcp port to receive StreamEvents.
   */
  int receivePort;

  /**
   * The inputs registered so far.
   */
  java.util.HashMap inputs = new java.util.HashMap();

  /**
   * The outputs registered so far.
   */
  java.util.HashMap outputs = new java.util.HashMap();

  /**
   * the tuple reception event handler.
   */
  TupleReceptionEventHandler handler = null;

  public static String getHostIpAddress(String ip) {
    if (ip == null || ip.equals("") || ip.equals("localhost") || ip.equals("127.0.0.1")) {
        try {
            String hostIp = java.net.InetAddress.getLocalHost().getHostAddress();
            ip = hostIp;
        } catch (Exception e) {
        }
    }
    return ip;
  }

  /**
   * Registers an input with its associated tuple class.
   * @param input the name of the Borealis input.
   * @param tupleClass the name of the tuple class.
   * @throws Exception if the specified tuple class is not available.
   */
  public static void register(String input, String tupleClass) throws Exception {
    Class c = Class.forName(tupleClass);
    input2class.put(input, c);
  }

  /**
   * Constructs a Marshal object.
   * @param ip the ip address of the Borealis node to talk with.
   * @param rpcPort the rpc port of the Borealis node to talk with.
   * @param receivePort my tcp port to receive Tuples.
   * @param handler the handler for the event of receiving a Tuple.
   */
  public Marshal(int receivePort, TupleReceptionEventHandler handler) {
    this.receivePort = receivePort;
    this.handler = handler;
    new StreamEventDispatcher().start();
  }

  /**
   * Establishes a connection to a Borealis input.
   * @param input the name of the Borealis input.
   * @param ip the ip address associated with the Borealis input.
   * @param port the port number associated with the Borealis input.
   * @throws Exception if cannot connect to the specified input.
   */
  public void connectInput(String input, String node) throws Exception {
    if (inputs.get(input) == null) {
      String ip = node.substring(0, node.indexOf(':'));
      ip = getHostIpAddress(ip);
      int port = Integer.parseInt(node.substring(node.indexOf(':')+1));
      System.out.println("input connection to " + input + " - " +
                         (ip + ":" + port));
      CCompatibleOutputStream os = new CCompatibleOutputStream(new java.net.
          Socket(ip, port + DATA_PORT_OFFSET).getOutputStream());
      InputStream is = new InputStream(input, os);
      inputs.put(input, is);
    }
  }

  /**
   * Adds a tuple to the batch of tuples bound for the specified input.
   * @param input the name of the Borealis input.
   * @param tuple the tuple to add to the batch of tuples.
   * @throws Exception if the connection to the specified input doesn't exist.
   */
  public void batchInput(String input, Tuple tuple) throws Exception {
    InputStream is = (InputStream) (inputs.get(input));
    if (is == null) {
      throw new Exception("the specified input doesn't exist");
    }
    else {
      is.sEvent.add(tuple);
    }
  }

  /**
   * Sends out the batch of tuples accumulated so far to the specified input (one tuple batch per input).
   * @param input the Borealis input to receive the tuple batch.
   * @throws Exception if the connection to the specified input doesn't exist or the transmission fails.
   */
  public void send(String input) throws Exception {
    InputStream is = (InputStream) (inputs.get(input));
    if (is == null) {
      throw new Exception("the specified input doesn't exist");
    }
    is.flush();
  }

  /**
   * Establishes a connection to a Borealis output.
   * @param output the name of the Borealis output.
   * @param node the borealis node to talk to.
   * @throws Exception if cannot connect to the specified output.
   */
  public void connectOutput(String output, String node) throws
      Exception {
    synchronized(outputs) {
      if (outputs.get(output) == null) {
        String ip = node.substring(0, node.indexOf(':'));
        ip = getHostIpAddress(ip);
        int rpcPort = Integer.parseInt(node.substring(node.indexOf(':')+1));
        java.net.Socket socket = new java.net.Socket(ip, rpcPort);
        CCompatibleOutputStream os = new CCompatibleOutputStream(socket.
            getOutputStream());
        Subscription s = new Subscription(output, java.net.InetAddress.getLocalHost().getHostAddress(), receivePort);
        s.write(os);
        os.flush();
        Consumer c = new Consumer(new java.io.DataInputStream(socket.getInputStream()));
        c.start();
        while(!c.isAcked()) {
          Thread.sleep(100);
        }
        outputs.put(output, output);
        System.out.println("output subscription " + s);
      }
    }
  }

  /**
   * Each InputStream object has meta-information for a Borealis input.
   */
  class InputStream {

    /**
     * The name of the Borealis input stream.
     */
    String input;

    /**
     * The output stream associated with the send socket.
     */
    CCompatibleOutputStream os;

    /**
     * The current StreamEvent
     */
    StreamEvent sEvent;

    /**
     * Constructs an InputStream.
     * @param input the Borealis input stream to send StreamEvents to.
     * @param os the socket output stream.
     * @throws Exception if a problem occurs.
     */
    public InputStream(String input, CCompatibleOutputStream os) throws
        Exception {
      this.input = input;
      this.os = os;
      setStreamEvent();
    }

    /**
     * Sets up a new StreamEvent for the input specified.
     */
    void setStreamEvent() {
      sEvent = new StreamEvent(input);
      sEvent.injected = 1;
    }

    /**
     * Writes out the current StreamEvent to the socket connected to a Borealis input.
     */
    void flush() throws Exception {
      sEvent.write(os);
      os.flush();
      setStreamEvent();
    }
  }

  class StreamEventDispatcher
      extends Thread {

    private class ReadThread
        extends Thread {
      java.net.Socket socket;

      public ReadThread(java.net.Socket socket) {
        this.socket = socket;
        System.out.println("opened " + socket);
      }

      public void run() {
        try {
          CCompatibleInputStream is = new CCompatibleInputStream(socket.
              getInputStream());
          while (true) {
            StreamEvent e = new StreamEvent();
            e.read(is);
            int n = e.tuples.size();
            for (int i = 0; i < n; i++) {
              if (handler != null)
                handler.handle(e.streamName, (Tuple) (e.tuples.elementAt(i)));
            }
          }
        }
        catch (Exception e) {
          e.printStackTrace();
        }
      }
    }

    public StreamEventDispatcher() {
    }

    public void run() {
      try {
        java.net.ServerSocket s = new java.net.ServerSocket(receivePort);
        while (true) {
          new ReadThread(s.accept()).start();
        }
      }
      catch (Exception e) {
        e.printStackTrace();
      }
    }
  }

  private class Consumer
      extends Thread {
    java.io.DataInputStream is;
    boolean isAcked = false;

    public Consumer(java.io.DataInputStream is) {
      this.is = is;
    }

    public boolean isAcked() {
      return isAcked;
    }

    public void run() {
      try {
        while (true) {
          byte b = is.readByte();
//          System.out.print((char)b);
          isAcked = true;
       }
      }
      catch (Exception e) {
        e.printStackTrace();
      }
    }
  }

}