/**************************************************************************
**   Author: hellojinjie
**   Date:   10/4/2012
**************************************************************************/

#ifndef TRAFFICSTATISTICS_H
#define TRAFFICSTATISTICS_H

#include <QWidget>

namespace Ui {
class TrafficStatistics;
}

class TrafficStatistics : public QWidget
{
    Q_OBJECT
    
public:
    explicit TrafficStatistics(QWidget *parent = 0);
    ~TrafficStatistics();
    
private:
    Ui::TrafficStatistics *ui;
};

#endif // TRAFFICSTATISTICS_H
