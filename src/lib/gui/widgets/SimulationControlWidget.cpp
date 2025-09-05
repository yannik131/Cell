#include "widgets/SimulationControlWidget.hpp"
#include "cell/Settings.hpp"
#include "ui_SimulationControlWidget.h"

SimulationControlWidget::SimulationControlWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SimulationControlWidget)
    , discDistributionPreviewTableModel_(new DiscDistributionPreviewTableModel(this))
{
    ui->setupUi(this);

    connect(ui->editDiscTypesPushButton, &QPushButton::clicked, [&]() { emit editDiscTypesClicked(); });
    connect(ui->editReactionsPushButton, &QPushButton::clicked, [&]() { emit editReactionsClicked(); });

    setRanges();
    displayGlobalSettings();
    setCallbacks();
}

void SimulationControlWidget::setRanges()
{
    ui->numberOfDiscsSpinBox->setRange(cell::SettingsLimits::MinNumberOfDiscs, cell::SettingsLimits::MaxNumberOfDiscs);
    ui->timeStepSpinBox->setRange(static_cast<int>(cell::SettingsLimits::MinSimulationTimeStep.asMicroseconds()),
                                  static_cast<int>(cell::SettingsLimits::MaxSimulationTimeStep.asMicroseconds()));
    ui->timeScaleDoubleSpinBox->setRange(cell::SettingsLimits::MinSimulationTimeScale,
                                         cell::SettingsLimits::MaxSimulationTimeScale);
    ui->cellWidthSpinBox->setRange(cell::SettingsLimits::MinCellWidth, cell::SettingsLimits::MaxCellWidth);
    ui->cellHeightSpinBox->setRange(cell::SettingsLimits::MinCellHeight, cell::SettingsLimits::MaxCellHeight);
}

void SimulationControlWidget::displayGlobalSettings()
{
}

void SimulationControlWidget::setCallbacks()
{
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

    ui->simulationSettingsWidget->setEnabled(!simulationStarted_);
}

void SimulationControlWidget::reset()
{
    emit simulationResetTriggered();
    ui->startStopButton->setText("Start");
    simulationStarted_ = false;
    ui->simulationSettingsWidget->setEnabled(true);
}
