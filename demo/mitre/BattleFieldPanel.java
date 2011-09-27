import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.awt.image.*;

public class BattleFieldPanel extends javax.swing.JPanel
{
  protected boolean firstDraw = true;
  public int width;
  public int height;
  Point virtualCenter;
  int virtualWidth;
  int virtualHeight;
  protected double magnification;

  BattleField battleField;

  protected int[] backgroundImage;
  protected BufferedImage bufferedImage;

  int prevX;
  int prevY;

  public BattleFieldPanel(BattleField battleField) {
    this.battleField = battleField;
    addMouseListener(
        new java.awt.event.MouseAdapter() {
      public void mousePressed(java.awt.event.MouseEvent e) {
        synchronized (this) {
          if (e.getClickCount() == 1) {
            if (e.getButton() == java.awt.event.MouseEvent.BUTTON1)
              zoomIn();
            else
              zoomOut();
          }
          else if (e.getClickCount() == 2) {
            if (e.getButton() == java.awt.event.MouseEvent.BUTTON1)
              magnification /= 1.2;
            else
              magnification *= 1.2;
            virtualCenter.x += (int) ( (e.getX() - width / 2.0) / magnification);
            virtualCenter.y +=
                (int) ( (e.getY() - height / 2.0) / magnification);
            reconstructBackground();
          }
          else if (e.getClickCount() == 3) {
            setNormalView();
          }
        }
      }
    });

  }

  public void zoomIn() {
    magnification *= 1.2;
    reconstructBackground();
  }

  public void zoomOut() {
    magnification /= 1.2;
    reconstructBackground();
  }

  public void setNormalView() {
    magnification = Math.min(1.0 * width / battleField.width,
                             1.0 * height / battleField.height);
    virtualCenter = new Point(battleField.width / 2, battleField.height / 2);
    virtualWidth = (int) (width / magnification);
    virtualHeight = (int) (height / magnification);
    reconstructBackground();
  }

  public void initialize() {
    width = 420;
    height = 420;
    System.out.println(width+"x"+height);
    backgroundImage = new int[width * height];
    bufferedImage = new BufferedImage(width, height,
                                      BufferedImage.TYPE_INT_ARGB);
    setNormalView();
  }

  public void reconstructBackground() {
    for (int i = 0; i < width; i++)
      for (int j = 0; j < height; j++) {
        double x = (i - width / 2.0) / magnification + virtualCenter.x;
        double y = (j - height / 2.0) / magnification + virtualCenter.y;
        int left = (int) (x);
        int right = (int) (x + .5);
        int top = (int) (y);
        int bottom = (int) (y + .5);
        if (left >= 0 && left < battleField.width &&
            right >= 0 && right < battleField.width &&
            top >= 0 && top < battleField.height &&
            bottom >= 0 && bottom < battleField.height) {
          int r = (battleField.rBackgroundImage[top * battleField.width + left] +
                   battleField.rBackgroundImage[top * battleField.width + right] +
                   battleField.rBackgroundImage[bottom * battleField.width +
                   left] +
                   battleField.rBackgroundImage[bottom * battleField.width +
                   right]) / 4;
          int g = (battleField.gBackgroundImage[top * battleField.width + left] +
                   battleField.gBackgroundImage[top * battleField.width + right] +
                   battleField.gBackgroundImage[bottom * battleField.width +
                   left] +
                   battleField.gBackgroundImage[bottom * battleField.width +
                   right]) / 4;
          int b = (battleField.bBackgroundImage[top * battleField.width + left] +
                   battleField.bBackgroundImage[top * battleField.width + right] +
                   battleField.bBackgroundImage[bottom * battleField.width +
                   left] +
                   battleField.bBackgroundImage[bottom * battleField.width +
                   right]) / 4;
          backgroundImage[j * width + i] = (255 << 24) |
              (r > 255 ? 255 : r) << 16 | (g > 255 ? 255 : g) << 8 |
              (b > 255 ? 255 : b);
        }
        else
          backgroundImage[j * width + i] = 255 << 24;
      }
  }

  public void paint(Graphics g) {
    super.paint(g);
    if (firstDraw) {
      firstDraw = false;
      initialize();
    }
    else {
      Dimension d = getSize();
    
      g.drawImage(bufferedImage, 0, 0, d.width - 1, d.height - 1, 0, 0,
                  width - 1,
                  height - 1, null);
      g.setColor(new Color(50, 50, 50));
      g.drawRect(0, 0, d.width-1, d.height-1);
    }
  }

  public void paint() {
    if (bufferedImage != null) {
      bufferedImage.setRGB(0, 0, width, height, backgroundImage, 0, width);
      Graphics g = bufferedImage.getGraphics();
      int rows = 10;
      int cols = 10;
      g.setColor(new Color(100, 100, 100));
      int left = (int) ( ( -virtualCenter.x) * magnification + width / 2.0);
      int right = (int) ( (battleField.width - virtualCenter.x) * magnification +
                         width / 2.0);
      int top = (int) ( ( -virtualCenter.y) * magnification + height / 2.0);
      int bottom = (int) ( (battleField.height - virtualCenter.y) *
                          magnification + height / 2.0);
      for (int i = 0; i <= cols; i++) {
        int x = (int) ( (i * battleField.width / cols - virtualCenter.x) *
                       magnification + width / 2.0);
        if (x >= 0 && x < width)
          g.drawLine(x, top, x, bottom);
      }
      for (int i = 0; i <= rows; i++) {
        int y = (int) ( (i * battleField.height / rows - virtualCenter.y) *
                       magnification + height / 2.0);
        if (y >= 0 && y < height)
          g.drawLine(left, y, right, y);
      }
    }
  }
}