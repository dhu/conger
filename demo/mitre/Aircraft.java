/*
 * @(#)Aircraft.java	1.0 04/18/2002
 *
 * Copyright 2001 by Jeong-Hyon Hwang (jhhwang@cs.brown.edu).
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

import java.awt.*;
import java.io.*;

/**
 * The Aircraft class encapsulates a number of common features of the Aircrafts in a cyber battle field.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/18/2002
 */

public class Aircraft extends BattleCraft
{
  public static int indexBase = 1000;

  protected static double[][][] image = {
    {{1,-1},{3,-1},{5,0},{1,0},{1,-1}},
    {{0,-1},{-5,0},{-6,-1},{-8,-1},{-6,1},{-1,1}},
    {{5,0},{4,1},{2,1}},
    {{0,0.5},{-2,2},{0,2},{2.5,.5},{0,0.5}},
    {{1,-1},{0,-2},{-1,-2},{0,-1},{1,-1}}
    };
  protected static double magnification = 1.7;
  protected static double colorChangeProbability = 1.0/12.0;

  protected static int moveInterval = 500;
  public static int velocity = 30;


  /**
   * Constructs a Aircraft object.
   * @param id the id of the Aircraft object.
   * @param x the x-position.
   * @param y the y-position.
   * @param field the battle field.
   */
  public Aircraft(int id, int x, int y, BattleField field, int color)
  {
    super(id, x, y, field, color);
  }

  /**
   * Returns a human-readable string representation [id - (x, y):[color] of this object.
   * @return a human-readable string representation of this object.
   */
  public String toString()
  {
    return "Aircraft "+super.toString();
  }

  /**
   * Draws this BattleCraft on the screen based on the specified paprameters.
   * @return a human-readable string representation of this object.
   */
  public void draw(Graphics g, int virtualCenterX, int virtualCenterY, double magnification, int viewWidth, int viewHeight)
  {
    int x = (int)((this.x-virtualCenterX)*magnification+viewWidth/2.0);
    int y = (int)((this.y-virtualCenterY)*magnification+viewHeight/2.0);
    draw(g, x, y);
  }

  /**
   * Draws this BattleCraft on the screen based on the specified paprameters.
   * @return a human-readable string representation of this object.
   */
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

