#include "dialogs/SetupDialog.hpp"
#include "cell/Settings.hpp"
#include "core/Utility.hpp"
#include "models/SetupModel.hpp"
#include "ui_SetupDialog.h"

SetupDialog::SetupDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : QDialog(parent)
    , ui(new Ui::SetupDialog)
    , setupModel_(new SetupModel(nullptr, simulationConfigUpdater))
    , simulationConfigUpdater_(simulationConfigUpdater)
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked,
            utility::safeSlot(this,
                              [this]()
                              {
                                  setupModel_->saveToConfig();
                                  accept();
                              }));
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(ui->useDistributionRadioButton, &QRadioButton::toggled, setupModel_, &SetupModel::setUseDistribution);
    connect(ui->timeStepSpinBox, &QSpinBox::valueChanged, setupModel_, &SetupModel::setTimeStepUs);
    connect(ui->timeScaleDoubleSpinBox, &QDoubleSpinBox::valueChanged, setupModel_, &SetupModel::setTimeScale);
    connect(ui->mostProbableSpeedSpinBox, &QSpinBox::valueChanged, setupModel_, &SetupModel::setMostProbableSpeed);
    connect(ui->FPSSpinBox, &QSpinBox::valueChanged, setupModel_, &SetupModel::setFPS);

    ui->timeStepSpinBox->setRange(static_cast<int>(cell::SettingsLimits::MinSimulationTimeStep.asMicroseconds()),
                                  static_cast<int>(cell::SettingsLimits::MaxSimulationTimeStep.asMicroseconds()));
    ui->timeScaleDoubleSpinBox->setRange(cell::SettingsLimits::MinSimulationTimeScale,
                                         cell::SettingsLimits::MaxSimulationTimeScale);
    ui->mostProbableSpeedSpinBox->setRange(static_cast<int>(cell::SettingsLimits::MinMostProbableSpeed),
                                           static_cast<int>(cell::SettingsLimits::MaxMostProbableSpeed));
    ui->FPSSpinBox->setRange(1, 240);

    displayCurrentConfig();
}

SetupDialog::~SetupDialog() = default;

void SetupDialog::showEvent(QShowEvent*)
{
    setupModel_->loadFromConfig();
    displayCurrentConfig();
}

void SetupDialog::displayCurrentConfig()
{
    const auto& config = simulationConfigUpdater_->getSimulationConfig();
    ui->useDistributionRadioButton->setChecked(config.useDistribution);
    ui->timeStepSpinBox->setValue(static_cast<int>(std::round(config.simulationTimeStep * 1e6)));
    ui->timeScaleDoubleSpinBox->setValue(config.simulationTimeScale);
    ui->mostProbableSpeedSpinBox->setValue(static_cast<int>(std::round(config.mostProbableSpeed)));
    ui->FPSSpinBox->setValue(simulationConfigUpdater_->getFPS());
}