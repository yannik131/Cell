#include "SimulationControlWidget.hpp"
#include "GlobalGUISettings.hpp"
#include "GlobalSettings.hpp"
#include "GlobalSettingsFunctor.hpp"
#include "ui_SimulationControlWidget.h"

SimulationControlWidget::SimulationControlWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SimulationControlWidget)
    , discDistributionPreviewTableModel_(new DiscDistributionPreviewTableModel(this))
{
    ui->setupUi(this);

    ui->discDistributionPreviewTableView->setModel(discDistributionPreviewTableModel_);

    setRanges();
    displayGlobalSettings();
    setCallbacks();
}

void SimulationControlWidget::setRanges()
{
    ui->fpsSpinBox->setRange(GUISettingsLimits::MinGuiFPS, GUISettingsLimits::MaxGuiFPS);
    ui->numberOfDiscsSpinBox->setRange(SettingsLimits::MinNumberOfDiscs, SettingsLimits::MaxNumberOfDiscs);
    ui->timeStepSpinBox->setRange(static_cast<int>(SettingsLimits::MinSimulationTimeStep.asMicroseconds()),
                                  static_cast<int>(SettingsLimits::MaxSimulationTimeStep.asMicroseconds()));
    ui->timeScaleDoubleSpinBox->setRange(SettingsLimits::MinSimulationTimeScale,
                                         SettingsLimits::MaxSimulationTimeScale);
}

void SimulationControlWidget::displayGlobalSettings()
{
    const auto& guiSettings = GlobalGUISettings::getGUISettings();
    ui->fpsSpinBox->setValue(guiSettings.guiFPS_);

    const auto& settings = GlobalSettings::getSettings();
    ui->numberOfDiscsSpinBox->setValue(settings.numberOfDiscs_);
    ui->timeStepSpinBox->setValue(static_cast<int>(settings.simulationTimeStep_.asMicroseconds()));
    ui->timeScaleDoubleSpinBox->setValue(settings.simulationTimeScale_);
}

void SimulationControlWidget::setCallbacks()
{
    // Connect callback for changed settings (after displaying the global settings, otherwise we
    // will trigger a world reset without having set the bounds first)
    connect(ui->fpsSpinBox, &QSpinBox::valueChanged, this, [this](int value)
            { tryExecuteWithExceptionHandling([=] { GlobalGUISettings::get().setGuiFPS(value); }, this); });

    connect(ui->numberOfDiscsSpinBox, &QSpinBox::valueChanged, this,
            [this](int value)
            {
                tryExecuteWithExceptionHandling(
                    [=]
                    {
                        GlobalSettings::get().setNumberOfDiscs(value);
                        emit simulationResetTriggered();
                    },
                    this);
            });

    connect(ui->timeStepSpinBox, &QSpinBox::valueChanged, this,
            [this](int value)
            {
                tryExecuteWithExceptionHandling(
                    [=] { GlobalSettings::get().setSimulationTimeStep(sf::microseconds(value)); }, this);
            });

    connect(ui->timeScaleDoubleSpinBox, &QDoubleSpinBox::valueChanged, this, [this](float value)
            { tryExecuteWithExceptionHandling([=] { GlobalSettings::get().setSimulationTimeScale(value); }, this); });

    connect(ui->editDiscTypesPushButton, &QPushButton::clicked, [this]() { emit editDiscTypesClicked(); });
    connect(ui->editReactionsPushButton, &QPushButton::clicked, [this]() { emit editReactionsClicked(); });

    connect(ui->startStopButton, &QPushButton::clicked, this, &SimulationControlWidget::toggleStartStopButtonState);
    connect(ui->resetButton, &QPushButton::clicked, this, &SimulationControlWidget::reset);
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
