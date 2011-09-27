package monitor;

import javax.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import org.apache.xerces.parsers.DOMParser;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2005</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class LayoutPanel
    extends JPanel {

  static double marginRatio = 0.15;
  static int titleHeight = 15;
  static Color defaultColor = Color.black;
  static Color backgroundColor = new Color(230, 230, 230);
  static Color failedNodeColor = new Color(150, 150, 150);
  static Color primaryNodeColor = new Color(255, 210, 210);
  static Color secondaryNodeColor = new Color(210, 210, 255);

  static int counter = 0;
  static int defaultColorMagnitude = 80;

  StatisticsCollector statCollector;
  static Image crashImage = Toolkit.getDefaultToolkit().getImage(monitor.
      MonitorFrame.class.getResource("dynamite.png"));

  static Color getColor() {
    int quotient = counter / 6;
    int ambient = defaultColorMagnitude + 50*quotient;
    int intensity = 240 - 50*quotient;
    int remainder = counter % 6;
    int r = intensity;
    int g = ambient;
    int b = ambient;
    switch (remainder) {
      case 1: r = ambient-40; g = intensity-40; b = ambient; break;
      case 2: r = intensity-40; g = intensity-40; b = ambient; break;
      case 3: r = ambient; g = intensity-40; b = intensity-40; break;
      case 4: r = ambient; g = ambient; b = intensity; break;
      case 5: r = intensity; g = ambient; b = intensity; break;
    }
    r = Math.min(255, Math.max(r, 0));
    g = Math.min(255, Math.max(g, 0));
    b = Math.min(255, Math.max(b, 0));
    counter++;
    if (ambient > intensity)
      counter = 0;
    return new Color(r, g, b);
  }

  public LayoutPanel(StatisticsCollector statCollector) {
    this.statCollector = statCollector;
    setBackground(backgroundColor);
    addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        if (e.isPopupTrigger()) {
          showPopupMenu(e);
        }
        else {
          chooseObject(e);
        }
      }
    });
  }

  void showPopupMenu(MouseEvent e) {
    Object object = getObject(e);
    if (object != null && object instanceof StatisticsCollector.Node) {
      StatisticsCollector.Node node = (StatisticsCollector.Node) object;
      JPopupMenu menu = new JPopupMenu();
      JMenuItem crashItem = new JMenuItem("Crash node " + node);
      crashItem.addActionListener(new ActionListnerForCrashMenu(node));
      menu.add(crashItem);
      JMenuItem setToCrashItem = new JMenuItem("Set to crash node " + node);
      setToCrashItem.addActionListener(new ActionListnerForSetToCrashMenu(node));
      menu.add(setToCrashItem);
      menu.show(e.getComponent(), e.getX(), e.getY());
    }
  }

  Object getObject(MouseEvent e) {
    Point point = e.getPoint();
    for (int i = 0; i < statCollector.nCols; i++) {
      Vector column = ( (Vector) (statCollector.nodeColumns.elementAt(i)));
      int n = column.size();
      for (int j = 0; j < n; j++) {
        StatisticsCollector.Node node = (StatisticsCollector.Node) (column.
            elementAt(j));
        if (node.rectangle.contains(e.getX(), e.getY())) {
          return node;
        }
      }
    }
    for (int i = 0; i < statCollector.outputStreamInfo.size(); i++) {
      StatisticsCollector.OutputStreamInfo outputStreamInfo = ( (
          StatisticsCollector.OutputStreamInfo) (statCollector.outputStreamInfo.
                                                 elementAt(i)));
      if (outputStreamInfo.rectangle.contains(e.getX(), e.getY())) {
        return outputStreamInfo;
      }
    }
    return null;
  }

  void chooseObject(MouseEvent e) {
    Object object = getObject(e);
    if (object == null) {
      return;
    }
    if (object instanceof StatisticsCollector.Node) {
      StatisticsCollector.Node node = (StatisticsCollector.Node) object;
      node.isDisplayOn = !node.isDisplayOn;
      statCollector.updatePanels();
    }
    else if (object instanceof StatisticsCollector.OutputStreamInfo) {
      StatisticsCollector.OutputStreamInfo outputStreamInfo = (
          StatisticsCollector.OutputStreamInfo) object;
      outputStreamInfo.isDisplayOn = !outputStreamInfo.isDisplayOn;
      statCollector.updatePanels();
    }
  }

  void crash(StatisticsCollector.Node node) {
    try {
      int answer = JOptionPane.showConfirmDialog(this,
          "Would you like to crash this node " + node + "?", "Kill Node?",
          JOptionPane.YES_NO_OPTION);
      if (answer == JOptionPane.YES_OPTION) {
        node.crash();
      }
    }
    catch (Exception e) {
      e.printStackTrace();
    }
  }

  class ActionListnerForCrashMenu
      implements ActionListener {
    StatisticsCollector.Node node;
    ActionListnerForCrashMenu(StatisticsCollector.Node node) {
      this.node = node;
    }

    public void actionPerformed(ActionEvent ev) {
      crash(node);
    };

  };

  class ActionListnerForSetToCrashMenu
      implements ActionListener {
    StatisticsCollector.Node node;
    ActionListnerForSetToCrashMenu(StatisticsCollector.Node node) {
      this.node = node;
    }

    public void actionPerformed(ActionEvent ev) {
      node.setToCrash();
    };

  };

  public void selectNode(MouseEvent e) {
    Point point = e.getPoint();
    for (int i = 0; i < statCollector.nCols; i++) {
      Vector column = ( (Vector) (statCollector.nodeColumns.elementAt(i)));
      int n = column.size();
      for (int j = 0; j < n; j++) {
        StatisticsCollector.Node node = (StatisticsCollector.Node) (column.
            elementAt(j));
        if (node.rectangle.contains(e.getX(), e.getY())) {
          node.isDisplayOn = !node.isDisplayOn;
        }
        statCollector.updatePanels();
      }
    }
  }

  public void paint(Graphics g) {
    synchronized (statCollector) {
      super.paint(g);
      int width = getWidth();
      int height = getHeight() - titleHeight;
      g.setColor(Color.gray);
      g.drawRect(0, 0, getWidth() - 1, getHeight() - 1);

      int xUnit = width / (statCollector.nCols + 1);
      int yUnit = height / statCollector.nRows;
      for (int i = 0; i < statCollector.nCols; i++) { // first-pass
        Vector column = ( (Vector) (statCollector.nodeColumns.elementAt(i)));
        int x = xUnit / 2 + i * width / (statCollector.nCols + 1);
        int n = column.size();
        for (int j = 0; j < n; j++) {
          int y = height / 2 - yUnit * n / 2 + yUnit * j + titleHeight;
          StatisticsCollector.Node node = (StatisticsCollector.Node) (column.
              elementAt(j));
          draw(g, node, x, y, xUnit, yUnit);
          node.drawnPair = false;
        }
      }
      g.setColor(defaultColor);
      for (int i = 0; i < statCollector.nCols; i++) { // second-pass
        Vector column = ( (Vector) (statCollector.nodeColumns.elementAt(i)));
        int n = column.size();
        for (int j = 0; j < n; j++) {
          StatisticsCollector.Node node = (StatisticsCollector.Node) (column.
              elementAt(j));
          if (!node.drawnPair && node.partner != null) {
            Object o = statCollector.nodes.get(node.partner);
            if (o != null) {
              StatisticsCollector.Node partner = (StatisticsCollector.Node)o;
              double minX = Math.min(node.rectangle.getMinX(),
                                     partner.rectangle.getMinX());
              double maxX = Math.max(node.rectangle.getMaxX(),
                                     partner.rectangle.getMaxX());
              double minY = Math.min(node.rectangle.getMinY(),
                                     partner.rectangle.getMinY());
              double maxY = Math.max(node.rectangle.getMaxY(),
                                     partner.rectangle.getMaxY());
              g.drawRect( (int) minX - 7, (int) minY - 20,
                         (int) (maxX - minX) + 7 + 7,
                         (int) (maxY - minY) + 20 + 7);
              g.drawString(node.haMethod, (int) minX - 7, (int) minY - 20 - 5);
            }
          }
        }
      }
      // draw output streams
      int size = statCollector.outputStreamInfo.size();
      if (size > 0) {
        yUnit = height / (size + 1);
        for (int i = 0; i < size; i++) {
          StatisticsCollector.OutputStreamInfo output = (StatisticsCollector.
              OutputStreamInfo)
              statCollector.outputStreamInfo.elementAt(i);
          g.setColor(output.color);
          for (int k = 0; k < output.subscriptions.size(); k++) {
            Point p = ( (StatisticsCollector.Node.Port) output.subscriptions.
                       elementAt(k)).point;
            g.drawLine(width - xUnit / 2, yUnit + yUnit * i, p.x, p.y);
          }
          String name = output.name;
          name = name.substring(name.lastIndexOf('/') + 1, name.length());
          int xx = width - xUnit / 2 + 5;
          int yy = yUnit + yUnit * i + 5;
          int w = g.getFontMetrics().stringWidth(name);
          int h = g.getFontMetrics().getHeight();
          output.rectangle = new Rectangle(xx, yy - h, w, h);
          g.drawString(name, xx, yy);
          if (output.isDisplayOn) {
            g.drawString(name, xx + 1, yy);
            g.drawString(name, xx, yy + 1);
            g.drawString(name, xx + 1, yy + 1);
          }
        }
      }

      // draw input streams
      size = statCollector.inputStreamInfo.size();
      if (size > 0) {
        yUnit = height / (size + 1);
        for (int i = 0; i < size; i++) {
          StatisticsCollector.StreamInfo input = (StatisticsCollector.
                                                  StreamInfo)
              statCollector.inputStreamInfo.elementAt(i);
          Object node = statCollector.nodes.get(input.node);
          if (node != null) {
            Object port = ( (StatisticsCollector.Node) node).streams.get(input.
                name);
            if (port != null) {
              g.setColor(defaultColor);

              Point p = ( (StatisticsCollector.Node.Port) port).point;
              String name = ( (StatisticsCollector.Node.Port) port).name;
              name = name.substring(name.lastIndexOf('/') + 1, name.length());
              int w = g.getFontMetrics().stringWidth(name);
              g.drawLine(xUnit / 2, yUnit + yUnit * i + titleHeight, p.x,
                         p.y);
              g.drawString(name, xUnit / 2 - w - 5,
                           yUnit + yUnit * i + 5 + titleHeight);
            }
          }
        }
      }
    }
  }

  protected void draw(Graphics g, StatisticsCollector.Node node, int x, int y,
                      int width, int height) {

    // fill the interior and put node name
    x = (int) (x + width * marginRatio);
    y = (int) (y + height * marginRatio);
    width = (int) (width * (1 - 2 * marginRatio));
    height = (int) (height * (1 - 2 * marginRatio)) - titleHeight;
    if (node.isActive) {
      if (node.isPrimary) {
        g.setColor(primaryNodeColor);
      }
      else {
        g.setColor(secondaryNodeColor);
      }
    }
    else {
      g.setColor(failedNodeColor);
    }
    int y1 = y + titleHeight;
    int y2 = y1 - 5;
    g.fillRect(x, y1, width, height);
    int spacing = 0;
    if (node.setToCrash) {
      spacing = crashImage.getWidth(this)+3;
      g.drawImage(crashImage, x - 1, y2 - 10, this);
    }
    g.setColor(Color.black);
    g.drawRect(x, y1, width, height);
    g.setColor(node.color);
    g.drawString(node.nodeDescription, x + spacing, y2);
    if (node.isDisplayOn) {
      g.drawRect(x-1, y1-1, width+2, height+2);
      g.drawString(node.nodeDescription, x - 1 + spacing, y2);
      g.drawString(node.nodeDescription, x - 1 + spacing, y2 - 1);
      g.drawString(node.nodeDescription, x + spacing, y2 - 1);
    }

    node.rectangle = new Rectangle(x, y1, width, height);

    // draw the boxes
    int xMargin = (int) (width * marginRatio / (2 * marginRatio + node.nColumns));
    width = width - 2 * xMargin;
    if (node.nColumns > 0 && node.nRows > 0) {
      int xUnit = width / node.nColumns;
      int yUnit = height / node.nRows;
      for (int i = 0; i < node.nColumns; i++) {
        Vector column = ( (Vector) (node.boxColumns.elementAt(i)));
        int xx = x + i * width / node.nColumns + xMargin;
        int n = column.size();
        for (int j = 0; j < n; j++) {
          int yy = y1 + height / 2 - yUnit * n / 2 + yUnit * j;
          StatisticsCollector.Node.Box box = (StatisticsCollector.Node.Box) (
              column.
              elementAt(j));
          draw(g, node.subscriptions, box, xx, yy, xUnit, yUnit);
        }
      }
    }
  }

  protected void draw(Graphics g, Hashtable subscriptions,
                      StatisticsCollector.Node.Box box, int x,
                      int y,
                      int width, int height) {

    // fill the interior and put node name
    x = (int) (x + width * marginRatio);
    y = (int) (y + height * marginRatio);
    width = (int) (width * (1 - 2 * marginRatio));
    height = (int) (height * (1 - 2 * marginRatio));
    g.setColor(defaultColor);
    g.drawRect(x, y, width, height);
    int yUnit = height / (box.inputs.size() + 1);
    for (int i = 0; i < box.inputs.size(); i++) {
      StatisticsCollector.Node.Port port = (StatisticsCollector.Node.Port) box.
          inputs.elementAt(i);
      if (port.column == 0) {
        port.point = new Point(x, y + yUnit * (i + 1));
        Object sub = subscriptions.get(port.name);
        if (sub != null) {
          StatisticsCollector.Node.Port p = (StatisticsCollector.Node.Port) sub;
          g.drawLine(p.point.x, p.point.y, port.point.x, port.point.y);
        }
      }
      else if (port.point != null) {
        g.drawLine(x, y + yUnit * (i + 1), port.point.x, port.point.y);
      }
    }
    String name = box.name;
    name = name.substring(name.lastIndexOf('/') + 1, name.length());
    int w = g.getFontMetrics().stringWidth(name);
    if (w < width - 2) {
      g.drawString(name, x + width / 2 - w / 2, y + height / 2 + 5);
    }
    yUnit = height / (box.outputs.size() + 1);
    for (int i = 0; i < box.outputs.size(); i++) {
      StatisticsCollector.Node.Port port = (StatisticsCollector.Node.Port) box.
          outputs.elementAt(i);
      port.point = new Point(x + width, y + yUnit * (i + 1));
    }
  }

}