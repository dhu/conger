/**************************************************************************
**   Author: hellojinjie
**   Date:   10/4/2012
**************************************************************************/

#include "trafficstatistics.h"
#include "ui_trafficstatistics.h"

TrafficStatistics::TrafficStatistics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrafficStatistics)
{
    ui->setupUi(this);
    ui->wayComboBox->addItem(tr("way"));
    ui->wayComboBox->addItem("1");

    ui->dirComboBox->addItem(tr("way"));
    ui->dirComboBox->addItem("west");
    ui->dirComboBox->addItem("east");

    ui->segComboBox->addItem(tr("seg"));
    for (int i = 1; i <= 100; i++)
    {
        ui->segComboBox->addItem(QString::number(i));
    }


}

TrafficStatistics::~TrafficStatistics()
{
    delete ui;
}
