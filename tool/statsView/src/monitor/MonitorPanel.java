package monitor;

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

public class MonitorPanel
    extends JPanel {

  Monitor m;
  int viewIndex = -1;

  protected BufferedImage bufferedImage;
  boolean firstDraw = true;
  int width;
  int height;
  double maxRate = 0.1; // Magda modified: Fixed max rate for simpler viewing
//  double maxRate = 4500; // Magda modified: Fixed max rate for simpler viewing
  static double maxCapacity = 2400;

  public MonitorPanel() {}

  public void setMonitor(Monitor m, int viewIndex) {
    this.m = m;
    this.viewIndex = viewIndex;
  }

  public void initialize() {
    width = getWidth();
    height = getHeight();
    bufferedImage = new BufferedImage(width, height,
                                      BufferedImage.TYPE_INT_ARGB);
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

  public void paint() {
    if (bufferedImage != null) {
      Graphics g = bufferedImage.getGraphics();

      int yOffset = 4 + 14 * m.nodeDescriptions.length;
      int xOffset = 5;
      int ySpacing = 18;

      int width = getWidth();
      int height = getHeight();

      g.setColor(new Color(50, 50, 50));
      g.fillRect(0, 0, width, height);

      g.setColor(new Color(70, 70, 70));
      g.fillRect(0, 0, width, yOffset);

      width -= 2*xOffset;
      height -= (yOffset + 2*ySpacing);

      double maxRate = 0;
      if (maxCapacity > 0)
        maxRate = maxCapacity;

      for (int i = 0; i < m.inputRates.length; i++) {
        for (int j = 0; j < m.inputRates[0].length; j++) {
          if (maxRate < m.inputRates[i][j]) {
            maxRate = m.inputRates[i][j];
          }
        }
      }

      double y = maxRate;
      if (y == 0)
        y += 0.001;
      double yy = height - (y * height / y);

      g.setColor(new Color(50, 80, 50));
      for (int i = 0; i <= m.inputRates[0].length; i++) { // the grid lines
        g.drawLine(xOffset + i * width / m.inputRates[0].length, yOffset + ySpacing + (int) yy,
                   xOffset + i * width / m.inputRates[0].length,
                   yOffset + ySpacing + height);
      }
      for (int i = 0; i <= 10; i++) { // the grid lines
        double ty = i * (height - yy) / 10 + yy;
        g.drawLine(xOffset, yOffset + ySpacing + (int) ty, xOffset + width,
                   yOffset + ySpacing + (int) ty);
      }

      // Magda added: max capacity lane
      if (maxCapacity < maxRate && maxCapacity > 0) {
        g.setColor(new Color(200, 200, 200));
        int ty = (int)(height * (1 - maxCapacity / maxRate));
        g.drawLine(xOffset, yOffset + ySpacing + ty, xOffset + width, yOffset + ySpacing + ty);
        g.drawString("" + (float) maxCapacity + " Kbps", 2, ty - 10);
      }
      // End magda added

      for (int k = 0; k < m.nodeDescriptions.length; k++) { // for each node
        long currentTime = System.currentTimeMillis();
        double[] inputRates = m.inputRates[k];
        long[] measurementTimes = m.measurementTimes[k];
        int cursor = m.cursor;
        int[] xs = new int[inputRates.length];
        int[] ys = new int[inputRates.length];
        for (int i = 0; i < inputRates.length; i++) { // the y-coordinate values of the input rate lines
          xs[i] = xOffset + width -
              (int) (1.0 * width *
                     (currentTime -
                      measurementTimes[ (i + cursor + 1) % inputRates.length]) /
                     m.monitoringSpan);
          ys[i] = yOffset + ySpacing + height - (int) (
              (inputRates[ (i + cursor + 1) % inputRates.length] * height / y));
        }
        g.setColor(m.colors[k]);
        ((Graphics2D)g).setStroke(new BasicStroke(2));
        for (int i = 0; i < ys.length - 1; i++) { // the curves representing "time vs input rate"
          g.drawLine(xs[i], ys[i], xs[i + 1], ys[i + 1]);
        }
        ((Graphics2D)g).setStroke(new BasicStroke(1));
        g.drawString(m.nodeDescriptions[k], xOffset, 14 + 14 * k); // display [ip address]:[port] for each node
        for (int j = 0; j < m.moveTimes[k].size(); j++) {
          long time = currentTime -
              ( (Long) (m.moveTimes[k].elementAt(j))).longValue();
          int pos = xOffset + width - (int) (1.0 * width * time / m.monitoringSpan);
          g.drawLine(pos, yOffset + ySpacing + (int) yy, pos, yOffset + ySpacing + height);
        }
      }
      g.setColor(new Color(170, 255, 170));
      g.drawString("" + (float) y + " Kbps", xOffset,
                   yOffset + ySpacing + (int) (yy) - 2);
      g.drawString("" +
                   ( ( (float) (m.monitoringSpan / 60)) *
                    0.001) +
                   " minute(s) ago", xOffset, yOffset + ySpacing + height + 14);
    }
  }
}