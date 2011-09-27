import clientApi.*;

/**
 * This program merges two data files (data.txt and video.dat) into mitre_demo.dat.
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
class DataFileMerger {

  java.io.LineNumberReader elementTuples;
  CCompatibleInputStream imageTuples;
  CCompatibleOutputStream os;
  int videoOffset = 18000;
  static int timestamp = -1;

  DataFileMerger() throws Exception {
    elementTuples = new java.io.LineNumberReader(new java.io.FileReader(
        "data.txt"));
    imageTuples = new CCompatibleInputStream(new java.io.FileInputStream(
        "video.dat"));
    os = new CCompatibleOutputStream(new java.io.FileOutputStream(
        "mitre_demo.dat"));
  }

  ImageData getNextImageData() {
    try {
      ImageData tuple = new ImageData();
      tuple.readWithoutHeader(imageTuples);
      tuple.timestamp += videoOffset;
      return tuple;
    }
    catch (Exception e) {
      return null;
    }
  }

  Element getNextElement() {
    try {
      String line = elementTuples.readLine();
      if (line == null) {
        return null;
      }
      java.util.StringTokenizer tokenizer = new java.util.StringTokenizer(line);
      Element tuple = new Element(Integer.parseInt(tokenizer.nextToken())*100,
                                  Integer.parseInt(tokenizer.nextToken()),
                                  Integer.parseInt(tokenizer.nextToken()),
                                  Integer.parseInt(tokenizer.nextToken()),
                                  Integer.parseInt(tokenizer.nextToken()),
                                  Integer.parseInt(tokenizer.nextToken()));
      return tuple;
    }
    catch (Exception e) {
      return null;
    }
  }

  void write(Element tuple) throws Exception {
    os.writeByte('e');
    tuple.writeWithoutHeader(os);
  }

  void write(ImageData tuple) throws Exception {
    os.writeByte('i');
    tuple.writeWithoutHeader(os);
  }

  /**
   * The main program.
   */
  public static void main(String[] args) throws Exception {

/*    CCompatibleInputStream imageTuples;
    CCompatibleOutputStream os;

    imageTuples = new CCompatibleInputStream(new java.io.FileInputStream(
        "video.dat"));
    os = new CCompatibleOutputStream(new java.io.FileOutputStream(
        "video2.dat"));

    while(true) {
      ImageData tuple = new ImageData();
      tuple.readWithoutHeader(imageTuples);
      tuple.timestamp /= 10;
      tuple.writeWithoutHeader(os);
    }
*/

    DataFileMerger merger = new DataFileMerger();

    Element elementTuple = merger.getNextElement();
    ImageData imageTuple = merger.getNextImageData();

    while (imageTuple != null || elementTuple != null) {
      if (imageTuple == null) {
        merger.write(elementTuple);
      if (timestamp < elementTuple.timestamp) {
        timestamp = elementTuple.timestamp;
        System.out.println("current time stamp = " + timestamp);
      }
        elementTuple = merger.getNextElement();
      }
      else if (elementTuple == null) {
        merger.write(imageTuple);
      if (timestamp < imageTuple.timestamp) {
        timestamp = imageTuple.timestamp;
        System.out.println("current time stamp = " + timestamp);
      }
        imageTuple = merger.getNextImageData();
      }
      else if (imageTuple.timestamp < elementTuple.timestamp) {
        merger.write(imageTuple);
      if (timestamp < imageTuple.timestamp) {
        timestamp = imageTuple.timestamp;
        System.out.println("current time stamp = " + timestamp);
      }
        imageTuple = merger.getNextImageData();
      }
      else {
        merger.write(elementTuple);
      if (timestamp < elementTuple.timestamp) {
        timestamp = elementTuple.timestamp;
        System.out.println("current time stamp = " + timestamp);
      }
        elementTuple = merger.getNextElement();
      }
    }
  }
}