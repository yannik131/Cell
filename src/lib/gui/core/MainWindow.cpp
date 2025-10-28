#include "core/MainWindow.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/Utility.hpp"
#include "dialogs/DiscTypesDialog.hpp"
#include "dialogs/DiscsDialog.hpp"
#include "dialogs/MembraneTypesDialog.hpp"
#include "dialogs/MembranesDialog.hpp"
#include "dialogs/PlotDataSelectionDialog.hpp"
#include "dialogs/ReactionsDialog.hpp"
#include "dialogs/SetupDialog.hpp"
#include "ui_MainWindow.h"

#include <glog/logging.h>

#include <QKeyEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , simulation_(new Simulation())
    , simulationConfigUpdater_(&simulation_->getSimulationConfigUpdater())
    , plotModel_(new PlotModel(this, simulation_.get()))
    , discTypesDialog_(new DiscTypesDialog(this, simulationConfigUpdater_))
    , discsDialog_(new DiscsDialog(this, simulationConfigUpdater_))
    , membraneTypesDialog_(new MembraneTypesDialog(this, simulationConfigUpdater_))
    , membranesDialog_(new MembranesDialog(this, simulationConfigUpdater_))
    , reactionsDialog_(new ReactionsDialog(this, simulationConfigUpdater_))
    , setupDialog_(new SetupDialog(this, simulationConfigUpdater_))
    , plotDataSelectionDialog_(new PlotDataSelectionDialog(this, simulationConfigUpdater_, plotModel_))
{
    ui->setupUi(this);

    connect(ui->simulationControlWidget, &SimulationControlWidget::simulationStartClicked,
            utility::safeSlot(this, [this]() { startSimulation(); }));
    connect(ui->simulationControlWidget, &SimulationControlWidget::simulationStopClicked, this,
            &MainWindow::stopSimulation);

    connect(ui->simulationControlWidget, &SimulationControlWidget::simulationResetTriggered, this,
            &MainWindow::resetSimulation);

    connect(ui->simulationControlWidget, &SimulationControlWidget::editDiscTypesClicked, discTypesDialog_,
            &QDialog::show);
    connect(ui->simulationControlWidget, &SimulationControlWidget::editDiscsClicked, discsDialog_, &QDialog::show);

    connect(ui->simulationControlWidget, &SimulationControlWidget::editMembraneTypesClicked, membraneTypesDialog_,
            &QDialog::show);
    connect(ui->simulationControlWidget, &SimulationControlWidget::editMembranesClicked, membranesDialog_,
            &QDialog::show);

    connect(ui->simulationControlWidget, &SimulationControlWidget::editReactionsClicked, reactionsDialog_,
            &QDialog::show);
    connect(ui->simulationControlWidget, &SimulationControlWidget::editSetupClicked, setupDialog_, &QDialog::show);

    ui->plotWidget->setModel(plotModel_);

    connect(ui->plotControlWidget, &PlotControlWidget::selectDiscTypesClicked, plotDataSelectionDialog_,
            &QDialog::show);

    connect(ui->saveSettingsAsJsonAction, &QAction::triggered, this, &MainWindow::saveSettingsAsJson);
    connect(ui->loadSettingsFromJsonAction, &QAction::triggered, this, &MainWindow::loadSettingsFromJson);

    resizeTimer_.setSingleShot(true);
    connect(&resizeTimer_, &QTimer::timeout, [this]() { simulation_->emitFrame(RedrawOnly{true}); });

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
                auto config = simulationConfigUpdater_->getSimulationConfig();
                config.cellMembraneType.radius = std::min(widgetSize.height() / 2, widgetSize.width() / 2);
                simulationConfigUpdater_->setSimulationConfig(config);

                ui->simulationWidget->resetView();
            });

    connect(simulation_.get(), &Simulation::frame, ui->simulationWidget,
            [&](const FrameDTO& frame)
            {
                ui->simulationWidget->render(frame, simulation_->getDiscTypeRegistry(),
                                             simulationConfigUpdater_->getDiscTypeColorMap());
            });
    ui->simulationWidget->setSimulationConfigUpdater(simulationConfigUpdater_);
    connect(ui->simulationWidget, &SimulationWidget::renderRequired,
            [this]() { simulation_->emitFrame(RedrawOnly{true}); });

    connect(simulation_.get(), &Simulation::frame, plotModel_, &PlotModel::processFrame);

    ui->plotControlWidget->setModel(plotModel_);
    connect(ui->plotControlWidget, &PlotControlWidget::selectDiscTypesClicked, plotDataSelectionDialog_,
            &QDialog::show);

    // Application-wide shortcuts so they work even when the widget is a separate window
    auto* scFull = new QShortcut(QKeySequence(Qt::Key_F), this);
    scFull->setContext(Qt::ApplicationShortcut);

    connect(scFull, &QShortcut::activated, this, &MainWindow::toggleSimulationFullscreen);
    connect(ui->simulationWidget, &SimulationWidget::requestExitFullscreen, this,
            &MainWindow::toggleSimulationFullscreen);

    // This will queue an event that will be handled as soon as the event loop is available
    QTimer::singleShot(0, this, [&]() { simulation_->rebuildContext(); });
}

void MainWindow::resetSimulation()
{
    stopSimulation();
    simulation_->rebuildContext();
    plotModel_->reset();
}

void MainWindow::saveSettingsAsJson()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save settings", "", "JSON Files (*.json)");
    if (fileName.isEmpty())
        return;

    try
    {
        simulationConfigUpdater_->saveConfigToFile(fs::path{fileName.toStdString()});
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
        simulationConfigUpdater_->loadConfigFromFile(fs::path{fileName.toStdString()});
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "Couldn't open file", e.what());
    }
}

void MainWindow::toggleSimulationFullscreen()
{
    ui->simulationWidget->toggleFullscreen();
    fullscreenIsToggled_ = !fullscreenIsToggled_;
    simulation_->emitFrame(RedrawOnly{true});
}

MainWindow::~MainWindow()
{
    if (simulationThread_ != nullptr)
    {
        simulationThread_->requestInterruption();
        simulationThread_->wait();
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (fullscreenIsToggled_)
    {
        QMessageBox::warning(this, "Warning", "You have to disable fullscreen mode before closing the app.");
        event->ignore();
        return;
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    // We don't want to continously redraw while the user is resizing the window because it might be costly
    resizeTimer_.start(50);

    QMainWindow::resizeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_F)
    {
        if (ui->simulationWidget->isFullScreen())
            ui->simulationWidget->showNormal();
        else
            ui->simulationWidget->showFullScreen();
    }
}

void MainWindow::startSimulation()
{
    if (simulationThread_ != nullptr)
        throw ExceptionWithLocation("Simulation can't be started: It's already running");

    if (!simulation_->cellIsBuilt())
        throw ExceptionWithLocation("Can't start simulation: Cell has not been built yet.");

    simulationThread_ = new QThread();
    simulation_->moveToThread(simulationThread_);

    connect(simulationThread_, &QThread::started, ui->simulationControlWidget,
            [&]() { ui->simulationControlWidget->updateWidgets(SimulationRunning{true}); });
    connect(simulationThread_, &QThread::finished, ui->simulationControlWidget,
            [&]() { ui->simulationControlWidget->updateWidgets(SimulationRunning{false}); });

    connect(simulationThread_, &QThread::started,
            [&]()
            {
                simulation_->run();
                simulation_->moveToThread(QCoreApplication::instance()->thread());
                simulationThread_->quit();
            });

    connect(simulationThread_, &QThread::finished, simulationThread_, &QThread::deleteLater);
    connect(simulationThread_, &QThread::finished, this, [&]() { simulationThread_ = nullptr; });

    simulationThread_->start();
}

void MainWindow::stopSimulation()
{
    if (simulationThread_)
        simulationThread_->requestInterruption();
}
