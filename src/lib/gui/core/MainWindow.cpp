#include "core/MainWindow.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/Utility.hpp"
#include "dialogs/DiscTypesDialog.hpp"
#include "dialogs/PlotDataSelectionDialog.hpp"
#include "dialogs/ReactionsDialog.hpp"
#include "dialogs/SetupDialog.hpp"
#include "ui_MainWindow.h"

#include <glog/logging.h>

#include "MainWindow.hpp"
#include <QKeyEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , simulation_(new Simulation())
    , plotModel_(new PlotModel(this, simulation_.get()))
    , discTypesDialog_(new DiscTypesDialog(this, simulation_.get()))
    , reactionsDialog_(new ReactionsDialog(this, simulation_.get()))
    , setupDialog_(new SetupDialog(this, simulation_.get()))
    , plotDataSelectionDialog_(new PlotDataSelectionDialog(this, simulation_.get(), plotModel_))
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
                auto config = simulation_->getSimulationConfig();
                config.setup.cellWidth = widgetSize.width();
                config.setup.cellHeight = widgetSize.height();
                simulation_->setSimulationConfig(config);

                ui->simulationWidget->resetView();
            });

    connect(simulation_.get(), &Simulation::frame, ui->simulationWidget,
            [&](const FrameDTO& frame)
            {
                ui->simulationWidget->render(frame, simulation_->getDiscTypeResolver(),
                                             simulation_->getDiscTypeColorMap());
            });
    ui->simulationWidget->injectAbstractSimulationBuilder(simulation_.get());
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
        simulation_->saveConfigToFile(fs::path{fileName.toStdString()});
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
        simulation_->loadConfigFromFile(fs::path{fileName.toStdString()});
    }
    catch (const ExceptionWithLocation& e)
    {
        QMessageBox::warning(this, "Couldn't open file", e.what());
    }
}

void MainWindow::toggleSimulationFullscreen()
{
    // This was ChatGPT-generated
    static QWidget* origParent = nullptr;
    static QLayout* origLayout = nullptr;
    static QWidget* placeholder = nullptr;
    static Qt::WindowFlags origFlags;

    QWidget* w = ui->simulationWidget;

    if (!placeholder)
    {
        // Detach to full screen
        origParent = w->parentWidget();
        origLayout = origParent ? origParent->layout() : nullptr;
        origFlags = w->windowFlags();

        if (origLayout)
        {
            placeholder = new QWidget(origParent);
            placeholder->setSizePolicy(w->sizePolicy());
            origLayout->replaceWidget(w, placeholder);
        }

        w->setParent(nullptr);
        w->setWindowFlag(Qt::Window, true);
        w->showFullScreen();
        w->raise();
        w->activateWindow();
        fullscreenIsToggled_ = true;
    }
    else
    {
        // Restore to original place
        w->showNormal(); // leave full screen
        w->setWindowFlags(origFlags & ~Qt::Window);
        w->setParent(origParent);

        if (origLayout)
        {
            origLayout->replaceWidget(placeholder, w);
            placeholder->deleteLater();
        }

        placeholder = nullptr;
        origParent = nullptr;
        origLayout = nullptr;

        w->show(); // apply new flags/parent
        fullscreenIsToggled_ = false;
    }

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

    if (!simulation_->contextIsBuilt())
        throw ExceptionWithLocation("Can't start simulation: Simulation context has not been built yet.");

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
