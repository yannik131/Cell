#include "MainWindow.hpp"
#include "SimulationSettingsWidget.hpp"
#include "ui_MainWindow.h"

#include <glog/logging.h>

#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , simulationThread_(nullptr)
    , simulation_(new Simulation(this))
    , discDistributionDialog_(new DiscTypeDistributionDialog(this))
    , reactionsDialog_(new ReactionsDialog(this))
    , plotDataSelectionDialog_(new PlotDataSelectionDialog(this))
    , plotModel_(new PlotModel(this))
    , discTypeDistributionTableModel_(new DiscTypeDistributionTableModel(this))
    , discDistributionPreviewTableModel_(new DiscDistributionPreviewTableModel(this))
    , reactionsTableModel_(new ReactionsTableModel(this))
{
    ui->setupUi(this);

    connect(simulation_, &Simulation::sceneData, ui->simulationWidget, &SimulationWidget::initialize);
    connect(simulation_, &Simulation::frameData, ui->simulationWidget, &SimulationWidget::render);
    connect(simulation_, &Simulation::frameData, plotModel_, &PlotModel::receiveFrameDTO);

    connect(ui->startStopButton, &QPushButton::clicked, this, &MainWindow::onStartStopButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, plotModel_, &PlotModel::reset);
    connect(ui->editDiscTypesPushButton, &QPushButton::clicked, discDistributionDialog_, &QDialog::show);
    connect(ui->editReactionsPushButton, &QPushButton::clicked, reactionsDialog_, &QDialog::show);
    connect(ui->plotTypeComboBox, &QComboBox::currentTextChanged, plotModel_, &PlotModel::setCurrentPlotCategory);
    connect(ui->selectDiscTypesPushButton, &QPushButton::clicked, plotDataSelectionDialog_, &QDialog::show);
    connect(plotDataSelectionDialog_, &PlotDataSelectionDialog::selectedDiscTypeNames, plotModel_,
            &PlotModel::receiveSelectedDiscTypeNames);

    connect(ui->simulationSettingsWidget, &SimulationSettingsWidget::settingsChanged, simulation_, &Simulation::reset);
    connect(ui->simulationSettingsWidget, &SimulationSettingsWidget::settingsChanged, ui->plotWidget,
            &PlotWidget::reset);
    connect(discDistributionDialog_, &DiscTypeDistributionDialog::discDistributionChanged, simulation_,
            &Simulation::reset);
    connect(discDistributionDialog_, &DiscTypeDistributionDialog::discDistributionChanged, ui->plotWidget,
            &PlotWidget::reset);
    connect(discDistributionDialog_, &DiscTypeDistributionDialog::discDistributionChanged, ui->simulationSettingsWidget,
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
                           simulation_->setWorldBounds(sf::Vector2f(simulationSize.width(), simulationSize.height()));
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

MainWindow::~MainWindow()
{
    delete ui;
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
