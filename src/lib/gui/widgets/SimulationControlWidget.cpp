#include "widgets/SimulationControlWidget.hpp"
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
    connect(ui->reinitializeButton, &QPushButton::clicked, this, &SimulationControlWidget::reset);
}

void SimulationControlWidget::toggleStartStopButtonState()
{
    if (simulationStarted_)
    {
        emit simulationStopClicked();
        ui->startStopButton->setText("Start");
        simulationStarted_ = false;
    }
    else
    {
        emit simulationStartClicked();
        ui->startStopButton->setText("Stop");
        simulationStarted_ = true;
    }
}

void SimulationControlWidget::reset()
{
    emit simulationResetTriggered();
    ui->startStopButton->setText("Start");
    simulationStarted_ = false;
}
