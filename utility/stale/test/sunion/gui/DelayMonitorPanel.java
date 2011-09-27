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

public class DelayMonitorPanel
    extends OutputMonitorPanel {

    int[] min_delays;
    int[] max_delays;
    final static int max_delay = 3000;
    long previousSeqNum;

  public DelayMonitorPanel() {
    min_delays = new int[bucketSize];
    max_delays = new int[bucketSize];
    cursor = max_delays.length - 1;
    currentSequenceNumber = cursor;
    previousSeqNum = -1;
  }

  public void update(int sequenceNumber, int delay) {
      
      // Do not update delay for old tuples
      if ( (sequenceNumber < previousSeqNum) ) {
          return;
      } 
      previousSeqNum = sequenceNumber;

      if (sequenceNumber > currentSequenceNumber) {
          int n = sequenceNumber - currentSequenceNumber;
          for (int i = 0; i < n; i++) {
              cursor++;
              cursor %= max_delays.length;
              min_delays[cursor] = -1;
              max_delays[cursor] = -1;
          }
          currentSequenceNumber = sequenceNumber;
      }
      if (sequenceNumber + max_delays.length > currentSequenceNumber) {
          int pos = (cursor - currentSequenceNumber + sequenceNumber) %
              max_delays.length;
          if (pos < 0) {
              pos += max_delays.length;
          }
          if (delay >= 0) {
              if (min_delays[pos] < 0) {
                  min_delays[pos] = delay;
              }
              else
                  if (min_delays[pos] > delay) {
                      min_delays[pos] = delay;
                  }
              if (max_delays[pos] < 0) {
                  max_delays[pos] = delay;
              }
              else
                  if (max_delays[pos] < delay) {
                      max_delays[pos] = delay;
                  }
          }
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

      int yOffset = 15;
      int xOffset = 5;
      int ySpacing = 0;

      g.setColor(new Color(50, 50, 50));
      g.fillRect(0, 0, width, height);

      int width = this.width - 2 * xOffset;
      int height = this.height - yOffset - 2 * ySpacing;
      int xsize = (width - (nChunks - 1) * chunkSpacing) / nCols;
      int ysize = height / nRows;

      xOffset += (width - (nChunks - 1) * chunkSpacing - (nCols * xsize)) / 2;
      yOffset += (ySpacing + (height - (nRows * ysize)) / 2);

      width = this.width - 2 * xOffset;
      height = this.height - 2 * yOffset;

      int endSeqNumber = (int) (Math.ceil(1.0 * (currentSequenceNumber + 1) /
                                          (nRows * nColsPerChunk))) * nRows *
          nColsPerChunk - 1;
      int startSeqNumber = endSeqNumber - bucketSize + 1;

      int pos = cursor - (currentSequenceNumber - startSeqNumber);

      if (pos < 0) {
        pos += max_delays.length;
      }

      //for (int k = 0; k < bucketSize; k++) {
      //if (max_delays[k] > max_delay) {
      //  max_delay = max_delays[k];
      //}
      //}

      g.setColor(new Color(0, 0, 0));
      for (int i = 0; i <= 10; i++) {
        int y = yOffset + i * height / 10;
        g.drawLine(xOffset, y, xOffset + width, y);
      }
      for (int i = 0; i <= 100; i++) {
        int x = xOffset + i * width / 100;
        g.drawLine(x, yOffset, x, yOffset + height);
      }

      g.setColor(new Color(200, 250, 200));

      if (max_delay > 0) {
        for (int k = 0; k < bucketSize; k++) {
          if (pos != cursor) {
            if (min_delays[pos] > 0) {
              int x = xOffset + width * k / bucketSize;
              int y = this.height - yOffset -
                  (int) (height * 1.0 * min_delays[pos] /
                         max_delay);
              g.drawLine(x, y, x, y);
            }
            if (max_delays[pos] > 0) {
              int x = xOffset + width * k / bucketSize;
              int y = this.height - yOffset -
                  (int) (height * 1.0 * max_delays[pos] /
                         max_delay);
              g.drawLine(x, y, x, y);
            }
            pos = (pos + 1) % max_delays.length;
          }
          else {
            break;
          }
        }
      }

      g.setColor(new Color(250, 200, 200));
      g.drawString("" + max_delay + "msec delay",
                   xOffset,
                   yOffset - 2);
      g.drawString("" + startSeqNumber,
                   xOffset,
                   yOffset + height + 12);
      g.drawString("" + endSeqNumber,
                   xOffset + width -
                   g.getFontMetrics().stringWidth("" + endSeqNumber),
                   yOffset + height + 12);
    }
  }
}
