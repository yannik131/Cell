#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <glog/logging.h>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), 
    ui(new Ui::MainWindow),
    simulationThread_(new QThread()),
    simulation_(new Simulation())
{
    ui->setupUi(this);

    connect(simulation_, &Simulation::sceneData, ui->simulationWidget, &SimulationWidget::initialize);
    connect(simulation_, &Simulation::frameData, ui->simulationWidget, &SimulationWidget::render);
    connect(simulation_, &Simulation::collisionData, this, &MainWindow::onCollisionData);

    simulation_->moveToThread(simulationThread_);

    connect(simulationThread_, &QThread::started, simulation_, &Simulation::run);

    simulationThread_->start();
}

void MainWindow::onCollisionData(int collisions)
{
    if(updateCount_++ == 0)
        return; //The first values are a little wonky
        
    ui->plotWidget->addDataPoint(collisions);
}
