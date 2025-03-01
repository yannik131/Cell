#include "MainWindow.hpp"
#include "SimulationSettingsWidget.hpp"
#include "ui_MainWindow.h"

#include <glog/logging.h>

#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , simulationThread_(nullptr)
    , simulation_(new Simulation())
    , discDistributionDialog_(new DiscTypesDialog())
{
    ui->setupUi(this);

    connect(simulation_, &Simulation::sceneData, ui->simulationWidget, &SimulationWidget::initialize);
    connect(simulation_, &Simulation::frameData, ui->simulationWidget, &SimulationWidget::render);
    connect(simulation_, &Simulation::updateData, ui->simulationWidget, &SimulationWidget::update);
    connect(simulation_, &Simulation::collisionData, ui->plotWidget, &AnalysisPlot::addDataPoint);
    connect(ui->startStopButton, &QPushButton::clicked, this, &MainWindow::onStartStopButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);
    connect(ui->setDiscDistributionPushButton, &QPushButton::clicked, discDistributionDialog_, &QDialog::show);

    connect(ui->simulationSettingsWidget, &SimulationSettingsWidget::settingsChanged, simulation_, &Simulation::reset);
    connect(ui->simulationSettingsWidget, &SimulationSettingsWidget::settingsChanged, ui->plotWidget,
            &AnalysisPlot::reset);
    connect(discDistributionDialog_, &DiscTypesDialog::discDistributionChanged, simulation_, &Simulation::reset);
    connect(discDistributionDialog_, &DiscTypesDialog::discDistributionChanged, ui->plotWidget, &AnalysisPlot::reset);
    connect(discDistributionDialog_, &DiscTypesDialog::discDistributionChanged, ui->simulationSettingsWidget,
            &SimulationSettingsWidget::updateDiscDistributionPreviewTableView);

    connect(&resizeTimer_, &QTimer::timeout,
            [&]()
            {
                const auto& simulationSize = ui->simulationWidget->size();
                simulation_->setWorldBounds(sf::Vector2f(simulationSize.width(), simulationSize.height()));
                simulation_->reset();
                ui->plotWidget->reset();
            });
    resizeTimer_.setSingleShot(true);

    // This will queue an event that will be handled as soon as the event loop is available
    QTimer::singleShot(0, this,
                       [this]()
                       {
                           const auto& simulationSize = ui->simulationWidget->size();

                           // I haven't figured out yet why the height returned by size() is 20px off..
                           // Maybe the RenderWindow reserves that height for the title bar? Probably OS dependent
                           // though
                           simulation_->setWorldBounds(
                               sf::Vector2f(simulationSize.width(), simulationSize.height() - 20));
                           simulation_->reset();
                           initialSizeSet_ = true;
                       });
}

void MainWindow::onStartStopButtonClicked()
{
    if (ui->startStopButton->text() == StartString)
    {
        try
        {
            startSimulation();
        }
        catch (const std::runtime_error& error)
        {
            QMessageBox::critical(this, "Error", error.what());
        }
    }
    else
    {
        stopSimulation();
    }
}

void MainWindow::onResetButtonClicked()
{
    if (simulationThread_ != nullptr)
        connect(simulationThread_, &QThread::finished, simulation_, &Simulation::reset, Qt::QueuedConnection);
    else
        simulation_->reset();

    ui->plotWidget->reset();

    stopSimulation();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    if (!initialSizeSet_)
    {
        event->ignore();
        return;
    }

    QMainWindow::resizeEvent(event);

    if (resizeTimer_.isActive())
        resizeTimer_.stop();

    resizeTimer_.start(100);
}

void MainWindow::startSimulation()
{
    if (simulationThread_ != nullptr)
        throw std::runtime_error("Simulation can't be started: It's already running");

    simulationThread_ = new QThread();
    simulation_->moveToThread(simulationThread_);

    connect(simulationThread_, &QThread::started,
            [&]()
            {
                simulation_->run();
                simulation_->moveToThread(QCoreApplication::instance()->thread());
                simulationThread_->quit();
            });

    connect(simulationThread_, &QThread::finished, simulationThread_, &QThread::deleteLater, Qt::QueuedConnection);
    connect(simulationThread_, &QThread::finished, this, [&]() { simulationThread_ = nullptr; });

    simulationThread_->start();

    // Resizing the window would change the world (haha) so we can't allow it during simulation
    setFixedSize(size());

    ui->simulationSettingsWidget->setEnabled(false);
    ui->startStopButton->setText(StopString);
}

void MainWindow::stopSimulation()
{
    if (simulationThread_)
        simulationThread_->requestInterruption();
    // Revert the fixed size to enable resizing again
    setMinimumSize(QSize(0, 0));
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    ui->simulationSettingsWidget->setEnabled(true);
    ui->startStopButton->setText(StartString);
}
