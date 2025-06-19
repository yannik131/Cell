#include "MainWindow.hpp"
#include "ExceptionWithLocation.hpp"
#include "GlobalSettings.hpp"
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

    connect(ui->saveSettingsAsJsonAction, &QAction::triggered, this, &MainWindow::saveSettingsAsJson);
    connect(ui->loadSettingsFromJsonAction, &QAction::triggered, this, &MainWindow::loadSettingsFromJson);

    resizeTimer_.setSingleShot(true);
    connect(&resizeTimer_, &QTimer::timeout, [this]() { simulation_->emitFrameData(); });

    connect(ui->simulationWidget, &SimulationWidget::renderRequired, [this]() { simulation_->emitFrameData(); });
    connect(ui->simulationControlWidget, &SimulationControlWidget::fitIntoViewRequested,
            [this]()
            {
                if (simulationThread_)
                {
                    QMessageBox::information(this, "Simulation is running",
                                             "Can't resize right now, simulation is running");
                    return;
                }
                const auto& widgetSize = ui->simulationWidget->size();

                cell::GlobalSettings::get().setCellSize(widgetSize.width(), widgetSize.height());
                ui->simulationWidget->resetView();
            });

    // This will queue an event that will be handled as soon as the event loop is available
    QTimer::singleShot(0, this, &MainWindow::loadDefaultSettings);
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

void MainWindow::saveSettingsAsJson()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save settings", "", "JSON Files (*.json)");
    if (fileName.isEmpty())
        return;

    try
    {
        cell::GlobalSettings::get().saveAsJson(fileName.toStdString());
    }
    catch (const ExceptionWithLocation& e)
    {
        QMessageBox::warning(this, "Couldn't save file", e.what());
    }
}

void MainWindow::loadSettingsFromJson()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open settings", "", "JSON Files (*.json)");

    if (fileName.isEmpty())
        return;

    try
    {
        cell::GlobalSettings::get().loadFromJson(fileName.toStdString());
    }
    catch (const ExceptionWithLocation& e)
    {
        QMessageBox::warning(this, "Couldn't open file", e.what());
    }
}

void MainWindow::loadDefaultSettings()
{
    fs::path cwd = fs::current_path();
    fs::path defaultSettingsFile = cwd / "defaultSettings.json";

    if (fs::exists(defaultSettingsFile))
        cell::GlobalSettings::get().loadFromJson(defaultSettingsFile);
}

MainWindow::~MainWindow() = default;

void MainWindow::resizeEvent(QResizeEvent* event)
{
    // We don't want to continously redraw while the user is resizing the window because it might be costly
    resizeTimer_.start(50);

    QMainWindow::resizeEvent(event);
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
