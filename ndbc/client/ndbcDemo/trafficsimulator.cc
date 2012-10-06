/**************************************************************************
**   Author: hellojinjie
**   Date:   10/3/2012
**************************************************************************/

#include "trafficsimulator.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLayout>
#include <QVBoxLayout>
#include <QLine>
#include <QPen>
#include <QDebug>
#include <QBrush>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include <QPointF>
#include <QTimeLine>
#include <QGraphicsItemAnimation>

TrafficSimulator::TrafficSimulator(QWidget *parent) :
    QWidget(parent)
{
    this->setFixedSize(600, 500);

    layout = new QVBoxLayout;
    this->setLayout(layout);

    scene = new QGraphicsScene;

    view = new QGraphicsView(scene);
    layout->addWidget(view);

    this->drawHighway();
    this->startSimulate();
}

void TrafficSimulator::startSimulate()
{
    QGraphicsPixmapItem* car1 = new QGraphicsPixmapItem(QPixmap(":/app/images/car1.jpg"));
    car1->setScale(0.25);
    car1->setPos(0, 13);
    this->scene->addItem(car1);

    QTimeLine *timer = new QTimeLine(5000);
    timer->setEasingCurve(QEasingCurve::Linear);
    timer->setFrameRange(0, 100);

    QGraphicsItemAnimation *animation = new QGraphicsItemAnimation;
    animation->setItem(car1);
    animation->setTimeLine(timer);

    for (int i = 0; i < 200; ++i)
    {
        animation->setPosAt(i / 200.0, QPointF((i / 200.0) * 520, 13));
    }

    timer->start();

}

void TrafficSimulator::drawHighway()
{
    /* 双黄线 */
    QPen yellowPen;
    yellowPen.setColor(QColor(210, 210, 20));
    yellowPen.setWidth(4);

    QLine yellowLine1;
    yellowLine1.setP1(QPoint(0, 0));
    yellowLine1.setP2(QPoint(570, 0));
    this->scene->addLine(yellowLine1, yellowPen);

    QLine yellowLine2;
    yellowLine2.setP1(QPoint(0, 8));
    yellowLine2.setP2(QPoint(570, 8));
    this->scene->addLine(yellowLine2, yellowPen);

    /* 白线 */
    QPen whitePen;
    whitePen.setColor(Qt::gray);
    whitePen.setWidth(1);

    QLine whiteLine1;
    whiteLine1.setP1(QPoint(0, -50));
    whiteLine1.setP2(QPoint(570, -50));
    this->scene->addLine(whiteLine1, whitePen);

    QLine whiteLine2;
    whiteLine2.setP1(QPoint(0, -100));
    whiteLine2.setP2(QPoint(570, -100));
    this->scene->addLine(whiteLine2, whitePen);

    whiteLine2.setP1(QPoint(0, -150));
    whiteLine2.setP2(QPoint(570, -150));
    this->scene->addLine(whiteLine2, whitePen);

    whiteLine2.setP1(QPoint(0, 58));
    whiteLine2.setP2(QPoint(570, 58));
    this->scene->addLine(whiteLine2, whitePen);

    whiteLine2.setP1(QPoint(0, 108));
    whiteLine2.setP2(QPoint(570, 108));
    this->scene->addLine(whiteLine2, whitePen);

    whiteLine2.setP1(QPoint(0, 158));
    whiteLine2.setP2(QPoint(570, 158));
    this->scene->addLine(whiteLine2, whitePen);

    /* 黑色 */
    QPen blackPen;
    blackPen.setWidth(2);
    blackPen.setColor(Qt::black);

    QLine blackLine;
    blackLine.setP1(QPoint(0, -200));
    blackLine.setP2(QPoint(570, -200));
    this->scene->addLine(blackLine, blackPen);

    blackLine.setP1(QPoint(0, 208));
    blackLine.setP2(QPoint(570, 208));
    this->scene->addLine(blackLine, blackPen);
}
