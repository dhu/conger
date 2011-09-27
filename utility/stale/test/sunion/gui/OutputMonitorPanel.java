import javax.swing.JPanel;
import java.awt.image.*;
import java.awt.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class OutputMonitorPanel
    extends JPanel {

  String outputPort;
  static int nColsPerChunk = 40;
  static int nChunks = 4;
  static int nCols = nColsPerChunk * nChunks;
  static int chunkSpacing = 3;
  static int nRows = 40;
  static int bucketSize = nCols * nRows;
  static int updateInterval = 250;
  int cursor;
  int[] counts;
  java.util.Hashtable versions2Colors = new java.util.Hashtable();
  Color[] colors;
  boolean darker = true;
  int currentSequenceNumber;
  int prevSequencenumber = -1;
  protected BufferedImage bufferedImage;
  boolean firstDraw = true;
  int width;
  int height;

  class UpdateThread
      extends Thread {

    OutputMonitorPanel p;

    public UpdateThread(OutputMonitorPanel p) {
      this.p = p;
    }

    public void run() {
      try {
        for (; ; ) {
          Thread.sleep(updateInterval);
          p.paint();
          p.repaint();
        }
      }
      catch (Exception e) {
        e.printStackTrace();
      }
    }
  }

  public OutputMonitorPanel() {
    counts = new int[bucketSize];
    colors = new Color[bucketSize];
    for (int i = 0; i < colors.length; i++) {
      colors[i] = null;
    }
    cursor = counts.length - 1;
    currentSequenceNumber = cursor;
  }

  public OutputMonitorPanel(String outputPort) {
    this();
    this.outputPort = outputPort;
  }

  public void initialize() {
    width = getWidth() - 1;
    height = getHeight() - 1;
    bufferedImage = new BufferedImage(width, height,
                                      BufferedImage.TYPE_INT_ARGB);
    new UpdateThread(this).start();
  }

  public void paint(Graphics g) {
    super.paint(g);
    if (firstDraw) {
      firstDraw = false;
      initialize();
    }
    else {
      g.drawImage(bufferedImage, 0, 0, null);
    }
  }

  public void update(String version, int sequenceNumber, String tupleType) {

      // RECONCILIATION_DONE will arrive with a zero sequence number so use previous tuple
      if  ( tupleType.equals("RECONCILIATION_DONE")) {
          sequenceNumber = prevSequencenumber;
      } 
      // Punctuation tuples have zero sequence numbers
      else if ( (sequenceNumber != 0) && (sequenceNumber > prevSequencenumber)) { 
          prevSequencenumber = sequenceNumber;
      }
      
      // Do nothing for sequence number zero or less
      if ( sequenceNumber < 1 ) {
          return;
      }

      if (sequenceNumber > currentSequenceNumber) {
          int n = sequenceNumber - currentSequenceNumber;
          for (int i = 0; i < n; i++) {
              cursor++;
              cursor %= counts.length;
              counts[cursor] = 0;
              colors[cursor] = null;
          }
          currentSequenceNumber = sequenceNumber;
      }


      if (sequenceNumber + counts.length > currentSequenceNumber) {
          int pos = (cursor - currentSequenceNumber + sequenceNumber) %
              counts.length;
          if (pos < 0) {
              pos += counts.length;
          }
      
          if ( tupleType.equals("INSERTION")) {
              int r = 150;
              int g = Math.max(0, 220 - 1/ 3 * 55);
              int b = 150;
              // Darker green when receiving corrections
              if ( (counts[pos] > 0) && darker ) {
                  r -= 100;
                  g -= 100;
                  b -= 100;
              }
              colors[pos] = new Color(r, g, b);
          } else if ( tupleType.equals("UNSTABLE") && (counts[pos] <= 0)) {
              colors[pos] = Color.YELLOW;
          } else {
              colors[pos] = Color.BLUE;
              if ( tupleType.equals("RECONCILIATION_DONE")) {
                  darker = false;
              }
              System.out.println("Set color to red");
          }
          counts[pos]++;
          prevSequencenumber = sequenceNumber;
      }
  }

  public void paint() {
    if (width != getWidth() || height != getHeight()) {
      width = getWidth() - 1;
      height = getHeight() - 1;
      bufferedImage = new BufferedImage(width, height,
                                        BufferedImage.TYPE_INT_ARGB);
    }
    if (bufferedImage != null) {
      Graphics g = bufferedImage.getGraphics();

      int yOffset = 4 + 14 + 14 * versions2Colors.size();
      int xOffset = 5;
      int ySpacing = 15;

      g.setColor(new Color(100, 100, 100));
      g.fillRect(0, 0, width, yOffset);

      g.setColor(new Color(50, 50, 50));
      g.fillRect(0, yOffset, width, height);

      java.util.Enumeration vers = versions2Colors.keys();
      g.setColor(new Color(200, 200, 200));

      int k = 0;
      g.drawString("port: " + outputPort, 12,
                   yOffset - 4 - 14 * versions2Colors.size());
      while (vers.hasMoreElements()) {
        String version = (String) (vers.nextElement());
        g.setColor( (Color) (versions2Colors.get(version)));
        g.drawString("version: " + version, 12, yOffset - 4 - 14 * (k++));
      }

      int width = this.width - 2 * xOffset;
      int height = this.height - yOffset - 2 * ySpacing;
      int xsize = (width - (nChunks - 1) * chunkSpacing) / nCols;
      int ysize = height / nRows;

      xOffset += (width - (nChunks - 1) * chunkSpacing - (nCols * xsize)) / 2;
      yOffset += (ySpacing + (height - (nRows * ysize)) / 2);

      int endSeqNumber = (int) (Math.ceil(1.0 * (currentSequenceNumber + 1) /
                                          (nRows * nColsPerChunk))) * nRows *
          nColsPerChunk - 1;
      int startSeqNumber = endSeqNumber - bucketSize + 1;

      int pos = cursor - (currentSequenceNumber - startSeqNumber);

      if (pos < 0) {
        pos += counts.length;
      }
      for (k = 0; k < nChunks; k++) {
        if (pos != cursor) {
          for (int j = 0; j < nRows; j++) {
            for (int i = 0; i < nColsPerChunk; i++) {
              if (counts[pos] > 0) {
                g.setColor(colors[pos]);
                g.fillRect(xOffset + (i + k * nColsPerChunk) * xsize +
                           k * chunkSpacing, yOffset + j * ysize, xsize,
                           ysize);
              }
              if (pos == cursor) {
                break;
              }
              pos = (pos + 1) % counts.length;
            }
            if (pos == cursor) {
              break;
            }
          }
        }

        g.setColor(new Color(0, 0, 0));
        for (int i = 0; i <= nRows; i++) {
          int x = xOffset + k * (chunkSpacing + nColsPerChunk * xsize);
          g.drawLine(x, yOffset + i * ysize, x + nColsPerChunk * xsize,
                     yOffset + i * ysize);
        }
        for (int i = 0; i <= nColsPerChunk; i++) {
          int x = xOffset + i * xsize +
              k * (chunkSpacing + nColsPerChunk * xsize);
          g.drawLine(x, yOffset, x,
                     yOffset + nRows * ysize);

        }
        g.setColor(new Color(250, 200, 200));

        g.drawString("" + (startSeqNumber+k*nRows*nColsPerChunk), xOffset+k*(nColsPerChunk*xsize+chunkSpacing), yOffset - 2);

        g.drawString("" + (startSeqNumber+(k+1)*nRows*nColsPerChunk-1),
                     xOffset+(k+1)*(nColsPerChunk*xsize+chunkSpacing) -chunkSpacing -
                     g.getFontMetrics().stringWidth("" + endSeqNumber),
                     yOffset + nRows * ysize + 12);

      }
    }
  }
}
