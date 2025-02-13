#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "SimulationSettingsWidget.hpp"

#include <glog/logging.h>

#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , simulationThread_(nullptr)
    , simulation_(new Simulation())
{
    ui->setupUi(this);

    connect(simulation_, &Simulation::sceneData, ui->simulationWidget, &SimulationWidget::initialize);
    connect(simulation_, &Simulation::frameData, ui->simulationWidget, &SimulationWidget::render);
    connect(simulation_, &Simulation::collisionData, ui->plotWidget, &AnalysisPlot::addDataPoint);
    connect(ui->startStopButton, &QPushButton::clicked, this, &MainWindow::onStartStopButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);

    connect(ui->simulationSettingsWidget, &SimulationSettingsWidget::settingsChanged, simulation_, &Simulation::setSimulationSettings);

    //This will queue an event that will be handled as soon as the event loop is available
    QTimer::singleShot(0, simulation_, &Simulation::reset);
}

void MainWindow::onStartStopButtonClicked()
{
    if (ui->startStopButton->text() == StartString)
    {
        try
        {
            startSimulation();
            ui->simulationSettingsWidget->lock();
            ui->startStopButton->setText(StopString);
        }
        catch (const std::runtime_error& error)
        {
            QMessageBox::critical(this, "Error", error.what());
        }
    }
    else
    {
        simulationThread_->requestInterruption();
        ui->simulationSettingsWidget->unlock();
        ui->startStopButton->setText(StartString);
    }
}

void MainWindow::onResetButtonClicked()
{
    if (simulationThread_ != nullptr)
    {
        connect(simulationThread_, &QThread::finished, simulation_, &Simulation::reset, Qt::QueuedConnection);
        simulationThread_->requestInterruption();
        ui->startStopButton->setText(StartString);
    }
    else 
        simulation_->reset();

    ui->plotWidget->reset();
    ui->simulationSettingsWidget->unlock();
}

void MainWindow::startSimulation()
{
    if (simulationThread_ != nullptr)
        throw std::runtime_error("Simulation can't be started: It's already running");

    simulationThread_ = new QThread();
    simulation_->moveToThread(simulationThread_);

    connect(simulationThread_, &QThread::started, [&]() {
        simulation_->run();
        simulation_->moveToThread(QCoreApplication::instance()->thread());
        simulationThread_->quit();
    });

    connect(simulationThread_, &QThread::finished, simulationThread_, &QThread::deleteLater, Qt::QueuedConnection);
    connect(simulationThread_, &QThread::finished, this, [&]() { 
        simulationThread_ = nullptr; 
    });

    simulationThread_->start();
}