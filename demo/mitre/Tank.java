/*
 * @(#)Tank.java	1.0 04/18/2002
 *
 * Copyright 2001 by Jeong-Hyon Hwang (jhhwang@cs.brown.edu).
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

import java.awt.*;
import java.io.*;

/**
 * The Tank class encapsulates a number of common features of the Tanks in a cyber battle field.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/18/2002
 */

public class Tank extends BattleCraft
{

  public static int indexBase = 0;

  protected static double[][][] image = {
    {{.5,-1}, {5,-2}},
    {{3,1},{1,1},{0,2},{2,2},{3,1},{2,0},{0,0},{1,1}},
    {{1.5,0},{1,-1.5},{-2,-1.5},{-3,0},{2,0}},
    {{-3,0},{-4,0},{-4,1},{-3,2},{2,2}}
    };
  protected static double magnification = 1.5;
  protected static double colorChangeProbability = 1.0/12.0;

  public static int moveInterval = 2000;
  public static int velocity = 5;

  /**
   * Constructs a Tank object.
   * @param id the id of the Tank object.
   * @param x the x-position.
   * @param y the y-position.
   * @param field the battle field.
   */
  public Tank(int id, int x, int y, BattleField field, int color)
  {
    super(id, x, y, field, color);
  }

  /**
   * Returns a human-readable string representation [id - (x, y):[color] of this object.
   * @return a human-readable string representation of this object.
   */
  public String toString()
  {
    return "Tank "+super.toString();
  }

  public void draw(Graphics g, int x, int y)
  {
    g.setColor(getColor());
    for (int i = 0; i < image.length; i++)
    {
      double[][] segment = image[i];
      for (int j = 1; j < segment.length; j++)
      {
        double sx = this.magnification*segment[j-1][0]+.5;
        double sy = this.magnification*segment[j-1][1]+.5;
        double ex = this.magnification*segment[j][0]+.5;
        double ey = this.magnification*segment[j][1]+.5;
        double angle = getAngle();
        int flag = -1;
        if (angle > -Math.PI/2 && angle < Math.PI/2)
          flag = 1;
        g.drawLine(x+(int)(sx*Math.cos(angle)-flag*sy*Math.sin(angle)), y+(int)(sx*Math.sin(angle)+flag*sy*Math.cos(angle)),
          x+(int)(ex*Math.cos(angle)-flag*ey*Math.sin(angle)), y+(int)(ex*Math.sin(angle)+flag*ey*Math.cos(angle)));
      }
    }
    g.drawLine(x, y, x, y);
  }

  protected void move()
  {
    int x = this.x+(int)(velocity*uX*moveInterval/1000);
    int y = this.y+(int)(velocity*uY*moveInterval/1000);

    moveTo(x, y);
  }
}

