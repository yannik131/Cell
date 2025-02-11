#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <glog/logging.h>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), 
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&retrieveCollisionsTimer_, &QTimer::timeout, this, &MainWindow::updatePlot);

    retrieveCollisionsTimer_.start(1000);
}

void MainWindow::updatePlot()
{
    int collisions = ui->simulationCanvas->getAndResetCollisions();

    if(updateCount_++ == 0)
        return; //The first values are a little wonky
        
    ui->plotWidget->addDataPoint(collisions);
}
