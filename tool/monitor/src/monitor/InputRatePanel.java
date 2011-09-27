package monitor;

import javax.swing.JPanel;
import java.awt.*;
import java.util.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2005</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class InputRatePanel
    extends JPanel {

  static Color defaultLineColor = new Color(210, 210, 210);
  static Color backgroundColor = new Color(230, 230, 230);
  static Color defaultTextColor = Color.black;
  static int nColumns = 100;
  static int nRows = 10;

  StatisticsCollector statCollector;

  public InputRatePanel(StatisticsCollector statCollector) {
    this.statCollector = statCollector;
    setBackground(backgroundColor);
  }

  public void paint(Graphics g) {
    synchronized (statCollector) {
      super.paint(g);
      int yOffset = 0;
      int xOffset = 5;
      int ySpacing = 18;

      int width = getWidth();
      int height = getHeight();
      g.setColor(Color.gray);
      g.drawRect(0, 0, width - 1, height - 1);

      width -= 2 * xOffset;
      height -= (yOffset + 2 * ySpacing);

      double maxRate = statCollector.getMaxInputRate();
      double y = maxRate;
      if (y == 0) {
        y += 0.001;
      }
      double yy = height - (y * height / y);

      g.setColor(defaultLineColor);
      for (int i = 0; i <= nColumns; i++) { // the grid lines
        int xx = xOffset + i * width / nColumns;
        g.drawLine(xx,
                   yOffset + ySpacing + (int) yy,
                   xx,
                   yOffset + ySpacing + height);
      }
      for (int i = 0; i <= nRows; i++) { // the grid lines
        double ty = i * (height - yy) / nRows + yy;
        g.drawLine(xOffset, yOffset + ySpacing + (int) ty, xOffset + width,
                   yOffset + ySpacing + (int) ty);
      }
      ( (Graphics2D) g).setStroke(new BasicStroke(2));
      for (int k = 0; k < statCollector.nCols; k++) {
        Vector column = ( (Vector) (statCollector.nodeColumns.elementAt(k)));
        int n = column.size();
        for (int j = 0; j < n; j++) {
          StatisticsCollector.Node node = (StatisticsCollector.Node) (column.
              elementAt(j));
          if (node.isDisplayOn) {
            long currentTime = System.currentTimeMillis();
            double[] inputRates = node.inputRates;
            long[] measurementTimes = node.measurementTimes;
            int cursor = statCollector.cursor;
            int[] xs = new int[inputRates.length];
            int[] ys = new int[inputRates.length];
            for (int i = 0; i < inputRates.length; i++) { // the y-coordinate values of the input rate lines
              xs[i] = xOffset + width -
                  (int) (1.0 * width *
                         (currentTime -
                          measurementTimes[ (i + cursor) % inputRates.length]) /
                         StatisticsCollector.monitoringTimeSpan);
              ys[i] = yOffset + ySpacing + height - (int) (
                  (inputRates[ (i + cursor) % inputRates.length] * height / y));
            }
            g.setColor(node.color);
            for (int i = 0; i < ys.length - 1; i++) { // the curves representing "time vs input rate"
              g.drawLine(xs[i], ys[i], xs[i + 1], ys[i + 1]);
            }
          }
        }
      }
      ( (Graphics2D) g).setStroke(new BasicStroke(1));
      g.setColor(defaultTextColor);
      String kBps = "" + (float) y;
      if (kBps.indexOf('.') + 2 < kBps.length()) {
        kBps = kBps.substring(0, kBps.indexOf('.') + 2);
      }
      g.drawString(kBps + " Kbps", xOffset,
                   yOffset + ySpacing + (int) (yy) - 4);
      g.drawString("" +
                   ( ( (float) (statCollector.monitoringTimeSpan / 60)) *
                    0.001) +
                   " minute(s) ago", xOffset, yOffset + ySpacing + height + 14);
    }
  }

}