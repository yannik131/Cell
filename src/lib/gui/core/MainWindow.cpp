#include "core/MainWindow.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "cell/SimulationContext.hpp"
#include "core/Utility.hpp"
#include "dialogs/DiscTypesDialog.hpp"
#include "dialogs/DiscsDialog.hpp"
#include "dialogs/MembraneTypesDialog.hpp"
#include "dialogs/MembranesDialog.hpp"
#include "dialogs/PlotDataSelectionDialog.hpp"
#include "dialogs/ReactionsDialog.hpp"
#include "dialogs/SetupDialog.hpp"
#include "ui_MainWindow.h"

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

    connect(ui->simulationWidget, &SimulationWidget::renderData, ui->simulationInfoWidget,
            &SimulationInfoWidget::setRenderData);

    ui->plotWidget->setModel(plotModel_);

    connect(ui->plotControlWidget, &PlotControlWidget::selectDiscTypesClicked, plotDataSelectionDialog_,
            &QDialog::show);

    connect(simulationConfigUpdater_, &SimulationConfigUpdater::simulationResetRequired, this,
            &MainWindow::resetSimulation);
    connect(simulationConfigUpdater_, &SimulationConfigUpdater::loopParameters, simulation_.get(),
            &Simulation::updateLoopParameters);

    connect(ui->saveSettingsAsJsonAction, &QAction::triggered, this, &MainWindow::saveSettingsAsJson);
    connect(ui->loadSettingsFromJsonAction, &QAction::triggered, this, &MainWindow::loadSettingsFromJson);
    connect(ui->aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);

    resizeTimer_.setSingleShot(true);
    connect(&resizeTimer_, &QTimer::timeout, ui->simulationWidget, &SimulationWidget::fitSimulationIntoView);

    connect(ui->simulationControlWidget, &SimulationControlWidget::fitIntoViewRequested, ui->simulationWidget,
            &SimulationWidget::fitSimulationIntoView);

    connect(simulation_.get(), &Simulation::initialFrame, ui->simulationWidget,
            &SimulationWidget::renderFrameImmediately);
    connect(simulation_.get(), &Simulation::frame, ui->simulationWidget, &SimulationWidget::queueFrameForRendering);
    connect(simulation_.get(), &Simulation::performanceData, ui->simulationInfoWidget,
            &SimulationInfoWidget::setPerformanceData);
    ui->simulationWidget->injectDependencies(simulationConfigUpdater_, simulation_.get());
    connect(simulation_.get(), &Simulation::dataPoint, plotModel_, &PlotModel::processDataPoint);

    connect(ui->simulationWidget, &SimulationWidget::renderRequired, simulation_.get(), &Simulation::emitLastFrame);

    connect(simulation_.get(), &Simulation::started, ui->simulationWidget, &SimulationWidget::startRenderingTimer);
    connect(simulation_.get(), &Simulation::stopped, ui->simulationWidget, &SimulationWidget::stopRenderingTimer);

    connect(simulation_.get(), &Simulation::started, ui->simulationControlWidget,
            [&]()
            {
                ui->menubar->setEnabled(false);
                ui->simulationControlWidget->updateWidgets(SimulationRunning{true});
            });

    connect(simulation_.get(), &Simulation::stopped, ui->simulationControlWidget,
            [&]()
            {
                ui->menubar->setEnabled(true);
                ui->simulationControlWidget->updateWidgets(SimulationRunning{false});
            });

    ui->plotControlWidget->setModel(plotModel_);
    connect(ui->plotControlWidget, &PlotControlWidget::selectDiscTypesClicked, plotDataSelectionDialog_,
            &QDialog::show);

    // Application-wide shortcuts so they work even when the widget is a separate window
    const auto addShortcut = [&](const QKeySequence& keySequence, auto callback)
    {
        auto* shortcut = new QShortcut(keySequence, this);
        shortcut->setContext(Qt::ApplicationShortcut);
        connect(shortcut, &QShortcut::activated, this, callback);
    };

    addShortcut(Qt::Key_F, &MainWindow::toggleSimulationFullscreen);
    addShortcut(Qt::Key_Space,
                [&]()
                {
                    if (simulation_->isRunning())
                        stopSimulation();
                    else
                        startSimulation();
                });

    connect(ui->simulationWidget, &SimulationWidget::requestExitFullscreen, this,
            &MainWindow::toggleSimulationFullscreen);

    // This will queue an event that will be handled as soon as the event loop is available
    QTimer::singleShot(0, this,
                       [&]()
                       {
                           resetSimulation();
                           ui->simulationWidget->fitSimulationIntoView();
                       });
}

void MainWindow::resetSimulation()
{
    simulation_->reinitialize();
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
        // Will emit a signal for simulation reset
        simulationConfigUpdater_->loadConfigFromFile(fs::path{fileName.toStdString()});
        ui->simulationWidget->fitSimulationIntoView();
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
}

void MainWindow::showAboutDialog()
{
    const QString aboutMessage = R"(
    <html>
        <head/>
        <body>
            <p>This is <b>Cell</b>, version 1.1.0. Build time: %1 %2.</p>
            <p>Application for simulating reaction networks.</p>
            <p>Developed by: Yannik Schroeder</p>
            <p>For additional information, see: <a href='https://github.com/yannik131/Cell'>https://github.com/yannik131/Cell</a></p>
        </body>
    </html>
)";

    QMessageBox::about(this, QStringLiteral("About"), aboutMessage.arg(__DATE__).arg(__TIME__));
}

MainWindow::~MainWindow()
{
    simulation_->stop();
    simulation_->waitForSimulationToFinish();
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

void MainWindow::startSimulation()
{
    if (simulation_->isRunning())
        throw ExceptionWithLocation("Simulation can't be started: It's already running");

    simulation_->start();
    ui->simulationControlWidget->updateWidgets(SimulationRunning{false});
}

void MainWindow::stopSimulation()
{
    simulation_->stop();
    ui->simulationControlWidget->updateWidgets(SimulationRunning{false});
}
