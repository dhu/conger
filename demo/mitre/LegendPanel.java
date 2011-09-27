import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.awt.image.*;

public class LegendPanel extends javax.swing.JPanel
{
//  Image image;
  public LegendPanel() {
    Toolkit toolkit = getToolkit();
  }

  public void paint(Graphics g) {
    Dimension d = getSize();
    g.setColor(new Color(190, 190, 190));
    g.fillRect(0, 0, d.width, d.height);
    g.setFont(new Font("Arial", Font.BOLD, 15));
    Aircraft airCraft = new Aircraft(0, 0, 0, null, 0);
    airCraft.draw(g, 30, 30);
    g.drawString("friendly aircraft", 50, 35);
    airCraft.setColor(1);
    airCraft.draw(g, 30, 50);
    g.drawString("enemy aircraft far", 50, 55);
    airCraft.setColor(4);
    airCraft.draw(g, 30, 70);
    g.drawString("enemy aircraft near", 50, 75);

    Tank tank = new Tank(0, 0, 0, null, 3);
    tank.draw(g, 24, 100);
    g.drawString("enemy tank far", 50, 105);
    tank.setColor(5);
    tank.draw(g, 24, 120);
    g.drawString("enemy tank near", 50, 125);
    tank.setColor(2);
    tank.draw(g, 24, 140);
    g.drawString("enemy tank in attack", 50, 145);
    tank.setColor(6);
    tank.draw(g, 24, 160);
    g.drawString("enemy aircraft in attack", 50, 165);
    Dimension iDim = new Dimension();
//    iDim.width = image.getWidth(this);
//    iDim.height = image.getHeight(this);
//    g.drawImage(image, d.width-iDim.width-10, d.height-iDim.height-10, this);

    g.setColor(new Color(50, 50, 50));
//    g.drawRect(d.width-iDim.width-10, d.height-iDim.height-10, iDim.width, iDim.height);
    g.drawRect(0, 0, d.width-1, d.height-1);
  }
}