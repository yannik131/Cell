#include "widgets/SimulationControlWidget.hpp"
#include "SimulationControlWidget.hpp"
#include "cell/Settings.hpp"
#include "ui_SimulationControlWidget.h"

SimulationControlWidget::SimulationControlWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SimulationControlWidget)
{
    ui->setupUi(this);

    connect(ui->discTypesPushButton, &QPushButton::clicked, [&]() { emit editDiscTypesClicked(); });
    connect(ui->discsPushButton, &QPushButton::clicked, [&]() { emit editDiscsClicked(); });
    connect(ui->membraneTypesPushButton, &QPushButton::clicked, [&]() { emit editMembraneTypesClicked(); });
    connect(ui->membranesPushButton, &QPushButton::clicked, [&]() { emit editMembranesClicked(); });
    connect(ui->reactionsPushButton, &QPushButton::clicked, [&]() { emit editReactionsClicked(); });
    connect(ui->setupPushButton, &QPushButton::clicked, [&]() { emit editSetupClicked(); });
    connect(ui->startStopButton, &QPushButton::clicked, this, &SimulationControlWidget::toggleStartStopButtonState);
    connect(ui->fitIntoViewButton, &QPushButton::clicked, [&]() { emit fitIntoViewRequested(); });
    connect(ui->reinitializeButton, &QPushButton::clicked, this, &SimulationControlWidget::reset);
}

SimulationControlWidget::~SimulationControlWidget() = default;

void SimulationControlWidget::updateWidgets(SimulationRunning simulationRunning)
{
    setWidgetsEnabled(!simulationRunning.value);
    if (simulationRunning.value)
        ui->startStopButton->setText("Stop");
    else
        ui->startStopButton->setText("Start");
}

void SimulationControlWidget::setWidgetsEnabled(bool value)
{
    ui->settingsWidget->setEnabled(value);
}

void SimulationControlWidget::toggleStartStopButtonState()
{
    if (ui->startStopButton->text() == "Start")
    {
        emit simulationStartClicked();
        ui->startStopButton->setText("Stop");
    }
    else
    {
        emit simulationStopClicked();
        ui->startStopButton->setText("Start");
    }
}

void SimulationControlWidget::reset()
{
    emit simulationResetTriggered();
    ui->startStopButton->setText("Start");
}
