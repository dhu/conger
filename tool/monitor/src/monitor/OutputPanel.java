package monitor;

import javax.swing.JPanel;
import javax.swing.ImageIcon;
import java.awt.*;
import java.awt.image.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2005</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class OutputPanel
    extends JPanel {
  StatisticsCollector.OutputStreamInfo output;

  static Color defaultLineColor = new Color(210, 210, 210);
  static Color backgroundColor = new Color(230, 230, 230);
  static Color defaultTextColor = Color.black;

  public OutputPanel(StatisticsCollector.OutputStreamInfo output) {
    setBackground(backgroundColor);
    this.output = output;
  }

  public void paint(Graphics g) {
    super.paint(g);
    synchronized (output) {
      paint(g, getWidth(), getHeight());
    }
  }

  public synchronized void paint(Graphics g, int width, int height) {

    int numberOfSections = StatisticsCollector.numberOfSections;
    int nTupleColumns = StatisticsCollector.nTupleColumns;
    int nTupleRows = StatisticsCollector.nTupleRows;
    int numberOfColumnsPerSection = StatisticsCollector.
        numberOfColumnsPerSection;
    int bucketSize = StatisticsCollector.bucketSize;
    int yOffset = 0;
    int xOffset = 1;
    int ySpacing = 15;
    int chunkSpacing = 3;

    width = width - 2 * xOffset;
    height = height - yOffset - 2 * ySpacing;

    int xsize = (width - (numberOfSections - 1) * chunkSpacing) /
        nTupleColumns;
    int ysize = height / nTupleRows;

    xOffset +=
        (width - (numberOfSections - 1) * chunkSpacing -
         (nTupleColumns * xsize)) / 2;
    yOffset += (ySpacing + (height - (nTupleRows * ysize)) / 2);

    int endSeqNumber = (int) (Math.ceil(1.0 * (output.currentSequenceNumber + 1) /
                                        (nTupleRows *
                                         numberOfColumnsPerSection))) *
        nTupleRows *
        numberOfColumnsPerSection - 1;
    int startSeqNumber = endSeqNumber - bucketSize + 1;

    int pos = output.cursor2 - (output.currentSequenceNumber - startSeqNumber);

    if (pos < 0) {
      pos += output.counts.length;
    }
    for (int k = 0; k < numberOfSections; k++) {
      if (pos != output.cursor2) {
        for (int j = 0; j < nTupleRows; j++) {
          for (int i = 0; i < numberOfColumnsPerSection; i++) {
            if (output.counts[pos] > 0) {
              g.setColor(output.colors[pos]);
              g.fillRect(xOffset +
                         (i + k * numberOfColumnsPerSection) * xsize +
                         k * chunkSpacing, yOffset + j * ysize, xsize,
                         ysize);
            }
            if (pos == output.cursor2) {
              break;
            }
            pos = (pos + 1) % output.counts.length;
          }
          if (pos == output.cursor2) {
            break;
          }
        }
      }

      g.setColor(defaultLineColor);
      for (int i = 0; i <= nTupleRows; i++) {
        int x = xOffset +
            k * (chunkSpacing + numberOfColumnsPerSection * xsize);
        g.drawLine(x, yOffset + i * ysize,
                   x + numberOfColumnsPerSection * xsize,
                   yOffset + i * ysize);
      }
      for (int i = 0; i <= numberOfColumnsPerSection; i++) {
        int x = xOffset + i * xsize +
            k * (chunkSpacing + numberOfColumnsPerSection * xsize);
        g.drawLine(x, yOffset, x,
                   yOffset + nTupleRows * ysize);

      }
      g.setColor(defaultTextColor);

      g.drawString("" +
                   (startSeqNumber + k * nTupleRows * numberOfColumnsPerSection),
                   xOffset +
                   k * (numberOfColumnsPerSection * xsize + chunkSpacing),
                   yOffset - 2);

      g.drawString("" +
                   (startSeqNumber +
                    (k + 1) * nTupleRows * numberOfColumnsPerSection -
                    1),
                   xOffset +
                   (k + 1) * (numberOfColumnsPerSection * xsize + chunkSpacing) -
                   chunkSpacing -
                   g.getFontMetrics().stringWidth("" + endSeqNumber),
                   yOffset + nTupleRows * ysize + 12);

    }
  }

}