/*
 * @(#)BattleCraft.java	1.0 04/18/2002
 *
 * Copyright 2001 by Jeong-Hyon Hwang (jhhwang@cs.brown.edu).
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

import java.awt.*;
import java.io.*;

/**
 * The BattleCraft class encapsulates a number of common features of the BattleCrafts in a cyber battle field.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/18/2002
 */

abstract public class BattleCraft
{
  protected static Color[] colors = {Color.blue, Color.yellow, Color.orange, Color.green, Color.cyan, Color.black, Color.red};
  public static String[] colorNames = {"blue", "brown", "orange", "green", "purple", "black", "red"};

  int colorIndex;
  protected int id;

  protected int prevX;
  protected int prevY;

  protected int x;
  protected int y;
  protected double uX;
  protected double uY;

  protected BattleField field;

  /**
   * Constructs a BattleCraft object.
   * @param id the id of the BattleCraft object.
   * @param x the x-position.
   * @param y the y-position.
   * @param field the battle field.
   */
  public BattleCraft(int id, int x, int y, BattleField field, int color)
  {
    this.id = id;
    this.prevX = x;
    this.prevY = y;
    this.x = x;
    this.y = y;
    this.field = field;
    this.colorIndex = color;
    setAngle();
  }

  /**
   * Returns a human-readable string representation [id - (x, y):[color] of this object.
   * @return a human-readable string representation of this object.
   */
  public String toString()
  {
    return ""+id+" - ("+x+", "+y+"): color("+ colorNames[colorIndex]+")";
  }

  /**
   * Gets the id of this BattleCraft object.
   * @return the id of this BattleCraft object.
   */
  public int getId()
  {
    return id;
  }

  /**
   * Gets the position of this BattleCraft object.
   * @return the position of this BattleCraft object.
   */
  public Point getPosition()
  {
    return new Point(x, y);
  }

  public double getAngle()
  {
    return Math.atan2(y - prevY, x - prevX);
  }

  protected void moveTo(int x, int y)
  {
    if (x < 0)
    {
      uX = - uX;
      x = 0;
    }
    if (x >= field.width)
    {
      uX = - uX;
      x = field.width-1;
    }
    if (y < 0)
    {
      uY = - uY;
      y = 0;
    }
    if (y >= field.height)
    {
      uY = - uY;
      y = field.height-1;
    }

    if (x != this.x || y != this.y)
    {
      prevX = this.x;
      prevY = this.y;
      this.x = x;
      this.y = y;
    }
  }

  public void set(int x, int y, int color)
  {
    moveTo(x, y);
    this.colorIndex = color;
  }

  public void randomlyChangeColor()
  {
    colorIndex = Math.min(colors.length-1, (int)(Math.random()*colors.length));
  }

  public void setColor(int color)
  {
    colorIndex = color;
  }

  public Color getColor()
  {
    return colors[colorIndex];
  }

  public int getColorCode()
  {
    return colorIndex;
  }

  protected void setAngle()
  {
    double angle = Math.random()*2*Math.PI;
    uX = Math.cos(angle);
    uY = Math.sin(angle);
  }

  abstract protected void move();

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
  abstract public void draw(Graphics g, int x, int y);

}

