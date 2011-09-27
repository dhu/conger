import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.awt.image.*;
import java.io.*;

public class BattleField
{
  public static int numberOfHills = 10;

  public int min_altitude = 0;
  public int max_altitude = 1000;

  static Color peakColor = new Color(90, 160, 100);
  static Color floorColor = new Color(60, 100, 60);

  protected int[][] altitudes = null;

  protected int[] rBackgroundImage;
  protected int[] gBackgroundImage;
  protected int[] bBackgroundImage;

  protected int width = 500;
  protected int height = 350;

  public BattleField()
  {
    setupAltitudes();

    rBackgroundImage = new int[width*height];
    gBackgroundImage = new int[width*height];
    bBackgroundImage = new int[width*height];

    setupBackground();
  }

  private void setupAltitudes()
  {
    int[] x_comps = new int[numberOfHills];
    int[] y_comps = new int[numberOfHills];
    int[] alts = new int[numberOfHills];

    for (int i = 0; i < numberOfHills; i++)
    {
      x_comps[i] = (int)(Math.random()*(width-2)+1);
      y_comps[i] = (int)(Math.random()*(height-2)+1);
      alts[i] = (numberOfHills+i)*100/(2*numberOfHills-1);
    }

    double[][] impact_sums = new double[width][height];

    double min_impact_sum = Double.MAX_VALUE;
    double max_impact_sum = Double.MIN_VALUE;

    for (int i = 0; i < width; i++)
    {
      for (int j = 0; j < height; j++)
      {
        double impact_sum = 0;
        for (int k = 0; k < numberOfHills; k++)
        {
          impact_sum = impact_sum + Math.pow((i-x_comps[k])*(i-x_comps[k])+(j-y_comps[k])*(j-y_comps[k])+10, -0.135);
        }
        impact_sums[i][j] = impact_sum;
        if (impact_sum < min_impact_sum)
          min_impact_sum = impact_sum;
        if (impact_sum > max_impact_sum)
          max_impact_sum = impact_sum;
      }
    }

    altitudes = new int[width][height];
    for (int i = 0; i < width; i++)
    {
      for (int j = 0; j < height; j++)
      {
        double l1 = impact_sums[i][j]-min_impact_sum;
        double l2 = max_impact_sum - impact_sums[i][j];
        int altitude = (int)((l1*max_altitude+l2*min_altitude)/(l1+l2));
        altitude = Math.max(altitude, min_altitude);
        altitude = Math.min(altitude, max_altitude);
        this.altitudes[i][j] = altitude;
      }
    }
  }

  protected void setupBackground()
  {
    int r1 = peakColor.getRed();
    int g1 = peakColor.getGreen();
    int b1 = peakColor.getBlue();

    int r2 = floorColor.getRed();
    int g2 = floorColor.getGreen();
    int b2 = floorColor.getBlue();

    for (int i = 0; i < width; i++)
      for (int j = 0; j < height; j++)
      {
        long l1 = altitudes[i][j]-min_altitude;
        long l2 = max_altitude-altitudes[i][j];

        rBackgroundImage[j*width+i] = (int)((l1*r1+l2*r2)/(l1+l2));
        gBackgroundImage[j*width+i] = (int)((l1*g1+l2*g2)/(l1+l2));
        bBackgroundImage[j*width+i] = (int)((l1*b1+l2*b2)/(l1+l2));
      }
  }
}