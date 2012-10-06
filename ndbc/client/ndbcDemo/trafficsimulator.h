/**************************************************************************
**   Author: hellojinjie
**   Date:   10/3/2012
**************************************************************************/

#ifndef TRAFFICSIMULATOR_H
#define TRAFFICSIMULATOR_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLayout>

class TrafficSimulator : public QWidget
{
    Q_OBJECT
public:
    explicit TrafficSimulator(QWidget *parent = 0);
    
signals:
    
public slots:
    
private:
    void drawHighway();
    void startSimulate();

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    QLayout* layout;

};

#endif // TRAFFICSIMULATOR_H
