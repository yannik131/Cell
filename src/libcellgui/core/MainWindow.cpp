#include "MainWindow.hpp"
#include "ExceptionWithLocation.hpp"
#include "GlobalSettingsFunctor.hpp"
#include "ui_MainWindow.h"

#include <glog/logging.h>

#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , simulation_(new Simulation())
    , discDistributionDialog_(new DiscTypeDistributionDialog(this))
    , reactionsDialog_(new ReactionsDialog(this))
    , plotDataSelectionDialog_(new PlotDataSelectionDialog(this))
    , plotModel_(new PlotModel(this))
{
    ui->setupUi(this);

    connect(simulation_.get(), &Simulation::frameData, ui->simulationWidget, &SimulationWidget::render);
    connect(simulation_.get(), &Simulation::frameData, plotModel_, &PlotModel::receiveFrameDTO);
    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::discTypeDistributionChanged, simulation_.get(),
            &Simulation::reset);
    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::numberOfDiscsChanged, simulation_.get(),
            &Simulation::reset);

    connect(ui->simulationControlWidget, &SimulationControlWidget::simulationStartClicked,
            [this]()
            {
                try
                {
                    startSimulation();
                }
                catch (const std::exception& e)
                {
                    QMessageBox::critical(this, "Error", "Error starting the simulation: " + QString(e.what()));
                }
            });
    connect(ui->simulationControlWidget, &SimulationControlWidget::simulationStopClicked, this,
            &MainWindow::stopSimulation);

    connect(ui->simulationControlWidget, &SimulationControlWidget::simulationResetTriggered, this,
            &MainWindow::resetSimulation);

    connect(ui->simulationControlWidget, &SimulationControlWidget::editDiscTypesClicked, discDistributionDialog_,
            &QDialog::show);
    connect(ui->simulationControlWidget, &SimulationControlWidget::editReactionsClicked, reactionsDialog_,
            &QDialog::show);

    ui->plotWidget->setModel(plotModel_);

    connect(ui->plotControlWidget, &PlotControlWidget::selectDiscTypesClicked, plotDataSelectionDialog_,
            &QDialog::show);

    connect(&resizeTimer_, &QTimer::timeout, this, &MainWindow::setSimulationWidgetSize);
    resizeTimer_.setSingleShot(true);

    // This will queue an event that will be handled as soon as the event loop is available
    QTimer::singleShot(0, this,
                       [this]()
                       {
                           setSimulationWidgetSize();
                           initialSizeSet_ = true;
                       });
}

void MainWindow::resetSimulation()
{
    stopSimulation();

    if (simulationThread_ != nullptr)
        connect(simulationThread_, &QThread::finished, simulation_.get(), &Simulation::reset, Qt::QueuedConnection);
    else
        simulation_->reset();

    plotModel_->clear();
}

void MainWindow::setSimulationWidgetSize()
{
    const auto& simulationSize = ui->simulationWidget->size();
    simulation_->setWorldBounds(
        sf::Vector2f(static_cast<float>(simulationSize.width()), static_cast<float>(simulationSize.height())));
    simulation_->reset();
    plotModel_->clear();
}

MainWindow::~MainWindow() = default;

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
        throw ExceptionWithLocation("Simulation can't be started: It's already running");

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
}

void MainWindow::stopSimulation()
{
    if (simulationThread_)
        simulationThread_->requestInterruption();

    // Revert the fixed size to enable resizing again
    setMinimumSize(QSize(0, 0));
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}
