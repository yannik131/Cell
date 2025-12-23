#include "dialogs/SetupDialog.hpp"
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

    ui->timeStepSpinBox->setRange(
        1, static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds{1}).count()));
    ui->timeScaleDoubleSpinBox->setRange(0, 100);
    ui->mostProbableSpeedSpinBox->setRange(0, static_cast<int>(1e6));
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
    ui->manualPositionsRadioButton->setChecked(!config.useDistribution);
    ui->timeStepSpinBox->setValue(static_cast<int>(std::round(config.simulationTimeStep * 1e6)));
    ui->timeScaleDoubleSpinBox->setValue(config.simulationTimeScale);
    ui->mostProbableSpeedSpinBox->setValue(static_cast<int>(std::round(config.mostProbableSpeed)));
    ui->FPSSpinBox->setValue(simulationConfigUpdater_->getFPS());
}