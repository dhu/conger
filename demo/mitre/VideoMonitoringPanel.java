import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.awt.image.*;
import java.io.*;
import java.util.*;

public class VideoMonitoringPanel
    extends javax.swing.JPanel
    implements Runnable {

  String title;
  VisualizerFrame monitor;
  Thread thread;
  long time = -1;
  public static int width = 352;
  public static int height = 240;
  BufferedImage[] imageBuffers;
  int current = 0;

  public VideoMonitoringPanel(VisualizerFrame parent,
                              String title) {
    this.title = title;
    this.monitor = parent;
    imageBuffers = new BufferedImage[2];
    imageBuffers[0] = new BufferedImage(width, height,
                                                BufferedImage.TYPE_INT_ARGB);
    imageBuffers[1] = new BufferedImage(width, height,
                                                BufferedImage.TYPE_INT_ARGB);
  }

  public void start() {
    thread = new Thread(this);
    thread.setPriority(Thread.MIN_PRIORITY);
    thread.setName("BattleFieldDisplay");
    thread.start();
  }

  public synchronized void stop() {
    thread = null;
    notify();
  }

  public void close() {
    monitor.removePanel(this);
  }

  public void update(ImageData tuple) {
    if (tuple.timestamp != time) {
      time = tuple.timestamp;
    }
//    System.out.println(tuple);
    current = tuple.frame_index % 2;
    for (int i = tuple.row_index_begin; i <= tuple.row_index_end; i++) {
      for (int j = 0; j < width; j++) {
        int r = (0xff & tuple.image_data[j]) << 16;
        int g = (0xff & tuple.image_data[j]) << 8;
        int b = (0xff & tuple.image_data[j]);
        int color = (255 << 24) + r + g + b;
        imageBuffers[current].setRGB(j, i, color);
      }
    }

  }

  public void paint(Graphics g) {
    Dimension d = getSize();
    super.paint(g);
    try {
    if (imageBuffers != null) {
      g.drawImage(imageBuffers[(current+1)%2], 0, 0, d.width - 1, d.height - 1, 0, 0,
                  width - 1,
                  height - 1, null);
    }
    g.setFont(new Font("Arial", Font.BOLD, 15));
    g.setColor(new Color(50, 50, 50));
    g.drawRect(0, 0, d.width - 1, d.height - 1);
    long current = Math.max(0, time);
    String text = title + " (";
    int min = (int) (current / 6000);
    if (min < 10) {
      text += "0";
    }
    text += min + ":";
    int sec = (int) (current % 6000);
    if (sec < 1000) {
      text += "0";
    }
    text += sec/100 + "." + (sec % 100) + ")";
    g.setColor(Color.black);
    g.drawString(text, 9, 29);
    g.drawString(text, 9, 30);
    g.drawString(text, 9, 31);
    g.drawString(text, 10, 29);
    g.drawString(text, 10, 30);
    g.drawString(text, 10, 31);
    g.drawString(text, 11, 29);
    g.drawString(text, 11, 30);
    g.drawString(text, 11, 31);
    g.setColor(Color.white);
    g.drawString(text, 10, 30);
    } catch (Exception e) {}
  }

  public void run() {
    Thread me = Thread.currentThread();
    while (thread == me && !isShowing() || getSize().width == 0) {
      try {
        thread.sleep(BattleFieldMonitoringPanel.refreshInterval);
      }
      catch (InterruptedException e) {
        return;
      }
    }

    while (thread == me && isShowing()) {
      repaint();
      try {
        thread.sleep(BattleFieldMonitoringPanel.refreshInterval);
      }
      catch (InterruptedException e) {
        break;
      }
    }
    thread = null;
  }
}