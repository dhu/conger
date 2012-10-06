#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "trafficsimulator.h"
#include "trafficstatistics.h"

#include <QAction>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLayout>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QAction* action = new QAction(tr("运行"), this);
    ui->mainToolBar->addAction(action);

    QGridLayout *centralLayout = new QGridLayout;
    ui->centralWidget->setLayout(centralLayout);

    TrafficStatistics* statistics = new TrafficStatistics;
    centralLayout->addWidget(statistics, 1, 0);

    TrafficSimulator* trafficSimulator = new TrafficSimulator;
    centralLayout->addWidget(trafficSimulator, 1, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
