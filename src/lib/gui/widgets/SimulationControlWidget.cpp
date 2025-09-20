#include "widgets/SimulationControlWidget.hpp"
#include "SimulationControlWidget.hpp"
#include "cell/Settings.hpp"
#include "ui_SimulationControlWidget.h"

SimulationControlWidget::SimulationControlWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SimulationControlWidget)
{
    ui->setupUi(this);

    connect(ui->editDiscTypesPushButton, &QPushButton::clicked, [&]() { emit editDiscTypesClicked(); });
    connect(ui->editReactionsPushButton, &QPushButton::clicked, [&]() { emit editReactionsClicked(); });
    connect(ui->editSetupPushButton, &QPushButton::clicked, [&]() { emit editSetupClicked(); });
    connect(ui->startStopButton, &QPushButton::clicked, this, &SimulationControlWidget::toggleStartStopButtonState);
    connect(ui->fitIntoViewButton, &QPushButton::clicked, [&]() { emit fitIntoViewRequested(); });
    connect(ui->reinitializeButton, &QPushButton::clicked, this, &SimulationControlWidget::reset);
}

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
    ui->editDiscTypesPushButton->setEnabled(value);
    ui->editReactionsPushButton->setEnabled(value);
    ui->editSetupPushButton->setEnabled(value);
    ui->fitIntoViewButton->setEnabled(value);
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
