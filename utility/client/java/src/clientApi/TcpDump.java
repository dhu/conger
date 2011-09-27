public class TcpDump
    extends Thread {

  static int count = 0;

  private class TcpDumpThread
      extends Thread {
    java.net.Socket socket;

    public TcpDumpThread(java.net.Socket socket) {
      this.socket = socket;
      System.out.println(socket);
    }

    public void run() {
      try {
        java.io.DataInputStream is = new java.io.DataInputStream(socket.
            getInputStream());

        String name = "rec";
        name += (count++);
        name += ".dat";
        java.io.DataOutputStream os = new java.io.DataOutputStream(new java.io.
            FileOutputStream(name));
        for (; ; ) {
          byte b = is.readByte();
          os.write(b);
          os.flush();
        }
      }
      catch (Exception e) {
        e.printStackTrace();
      }
    }
  }

  public TcpDump(java.net.ServerSocket server) throws Exception {
    new TcpDumpThread(server.accept()).start();
  }

  public static void main(String[] args) throws Exception {
    try {
      int port = 40000;
      System.out.println("server socket at port " + port);
      java.net.ServerSocket s = new java.net.ServerSocket(port);
      do {
        try {
          new TcpDump(s);
        }
        catch (Exception e) {
          e.printStackTrace();
        }
      }
      while (true);
    }
    catch (Exception e) {
      e.printStackTrace();
    }
  }
}