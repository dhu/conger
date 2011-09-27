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

public class DelayPanel
    extends JPanel {

  static Color defaultLineColor = new Color(210, 210, 210);
  static Color backgroundColor = new Color(230, 230, 230);
  static Color defaultTextColor = Color.black;

  StatisticsCollector statCollector;

  int nColumns = 100;

  public DelayPanel(StatisticsCollector statCollector) {
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
      g.drawRect(0, 0, getWidth() - 1, getHeight() - 1);

      width -= 2 * xOffset;
      height -= (yOffset + 2 * ySpacing);
      double maxDelay = statCollector.getMaxDelay();
      double y = maxDelay;
      if (y == 0) {
        y += 0.001;
      }
      double yy = height - (y * height / y);

      g.setColor(defaultLineColor);
      for (int i = 0; i <= nColumns; i++) { // the grid lines
        g.drawLine(xOffset + i * width / nColumns,
                   yOffset + ySpacing + (int) yy,
                   xOffset + i * width / nColumns,
                   yOffset + ySpacing + height);
      }
      for (int i = 0; i <= 10; i++) { // the grid lines
        double ty = i * (height - yy) / 10 + yy;
        g.drawLine(xOffset, yOffset + ySpacing + (int) ty, xOffset + width,
                   yOffset + ySpacing + (int) ty);
      }

      long timeSpan = System.currentTimeMillis();
      long oldTime = statCollector.getOldestTime();
      if (oldTime == 0) {
        timeSpan = 0;
      }
      else {
        timeSpan -= oldTime;
      }
      if (timeSpan > 0) {
        timeSpan = (long) (timeSpan * StatisticsCollector.bucketSize /
                           statCollector.getMaxbucketSize());
        // plot the curves (one for each output)
        int size = statCollector.outputStreamInfo.size();
        ( (Graphics2D) g).setStroke(new BasicStroke(4));
        for (int i = 0; i < size; i++) {
          StatisticsCollector.OutputStreamInfo streamInfo = (
              StatisticsCollector.
              OutputStreamInfo) statCollector.outputStreamInfo.elementAt(i);
          if (streamInfo.isDisplayOn) {
            long currentTime = System.currentTimeMillis();
            long[] delays = streamInfo.delays;
            long[] timestamps = streamInfo.timestamps;
            int cursor = streamInfo.cursor;
            int[] xs = new int[delays.length];
            int[] ys = new int[delays.length];
            int count = 0;
            for (int j = 0; j < delays.length; j++) { // the y-coordinate values of the input rate lines
              int index = (j + cursor) % delays.length;
              if (delays[index] >= 0) {
                xs[count] = xOffset + width -
                    (int) (1.0 * width *
                           (currentTime -
                            timestamps[index]) /
                           timeSpan);
                ys[count] = yOffset + ySpacing + height - (int) (
                    (delays[index] * height / y));
                count++;
              }
            }
            g.setColor(streamInfo.color);
            for (int j = 0; j < count; j++) { // the curves representing "time vs input rate"
              g.drawLine(xs[j], ys[j], xs[j], ys[j]);
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
      g.drawString(kBps + " msec", xOffset,
                   yOffset + ySpacing + (int) (yy) - 4);

      kBps = "" + ( (float) timeSpan) *
          0.001;
      if (kBps.indexOf('.') + 2 < kBps.length()) {
        kBps = kBps.substring(0, kBps.indexOf('.') + 2);
      }
      g.drawString(kBps +
                   " second(s) ago", xOffset, yOffset + ySpacing + height + 14);
    }
  }
}