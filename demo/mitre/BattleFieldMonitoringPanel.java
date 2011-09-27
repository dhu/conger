import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.io.*;

public class BattleFieldMonitoringPanel
    extends BattleFieldPanel
    implements Runnable {

  Thread thread;
  public static int refreshInterval = 1000;
  String title;
  VisualizerFrame monitor;
  long time = -1;
  
  Hashtable crafts = new Hashtable();

  public BattleFieldMonitoringPanel(VisualizerFrame parent,
                                    BattleField battleField, String title) {
    super(battleField);
    this.title = title;
    this.monitor = parent;
  }

  public void run() {
    Thread me = Thread.currentThread();
    while (thread == me && !isShowing() || getSize().width == 0) {
      try {
        thread.sleep(refreshInterval);
      }
      catch (InterruptedException e) {
        return;
      }
    }

    while (thread == me && isShowing()) {
      paint();
      repaint();
      try {
        thread.sleep(refreshInterval);
      }
      catch (InterruptedException e) {
        break;
      }
    }
    thread = null;
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

  public void paint() {
    super.paint();
    if (bufferedImage != null) {
      Graphics g = bufferedImage.getGraphics();
      synchronized (crafts) {
        for (Enumeration e = crafts.elements(); e.hasMoreElements(); ) {
          ( (BattleCraft) e.nextElement()).draw(g, virtualCenter.x,
                                                virtualCenter.y, magnification,
                                                width, height);
        }
      }
      g.setFont(new Font("Arial", Font.BOLD, 15));
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
    }
  }

  public void update(AppElement tuple) {
    if (tuple.timestamp != time) {
      repaint();
      time = tuple.timestamp;
    }
    BattleCraft craft = (BattleCraft) (crafts.get(new Integer(tuple.id)));
    if (craft == null) {
      if (tuple.id >= Aircraft.indexBase) {
        craft = new Aircraft(tuple.id, tuple.x, tuple.y,
                             battleField, tuple.color);
        crafts.put(new Integer(tuple.id), craft);
      }
      else if (tuple.id >= Tank.indexBase) {
        craft = new Tank(tuple.id, tuple.x, tuple.y,
                         battleField, tuple.color);
        crafts.put(new Integer(tuple.id), craft);
      }
    }
    else {
      craft.set(tuple.x, tuple.y, tuple.color);
    }
  }

  public void close() {
    monitor.removePanel(this);
  }

}