#include "dialogs/SetupDialog.hpp"
#include "cell/Settings.hpp"
#include "core/Utility.hpp"
#include "models/DiscTableModel.hpp"
#include "models/DiscTypeDistributionTableModel.hpp"
#include "ui_SetupDialog.h"

SetupDialog::SetupDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QDialog(parent)
    , ui(new Ui::SetupDialog)
    , discTypeDistributionTableModel_(new DiscTypeDistributionTableModel(this))
    , discTableModel_(new DiscTableModel(this))
    , setupModel_(new SetupModel(this, discTypeDistributionTableModel_, discTableModel_, abstractSimulationBuilder))
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked,
            utility::safeSlot(this,
                              [this]()
                              {
                                  setupModel_->commitChanges();
                                  hide();
                              }));
    connect(ui->cancelPushButton, &QPushButton::clicked,
            utility::safeSlot(this,
                              [this]()
                              {
                                  setupModel_->discardChanges();
                                  hide();
                              }));

    auto updateWidgets = [this](bool toggled)
    {
        bool enableDistributionWidget = toggled;
        bool enableManualWidget = !toggled;

        ui->distributionWidget->setEnabled(enableDistributionWidget);
        ui->manualWidget->setEnabled(enableManualWidget);
    };

    connect(ui->useDistributionRadioButton, &QRadioButton::toggled, updateWidgets);
    ui->useDistributionRadioButton->setChecked(true);
    updateWidgets(true);

    ui->numberOfDiscsSpinBox->setRange(cell::SettingsLimits::MinNumberOfDiscs, cell::SettingsLimits::MaxNumberOfDiscs);
    ui->timeStepSpinBox->setRange(static_cast<int>(cell::SettingsLimits::MinSimulationTimeStep.asMicroseconds()),
                                  static_cast<int>(cell::SettingsLimits::MaxSimulationTimeStep.asMicroseconds()));
    ui->timeScaleDoubleSpinBox->setRange(cell::SettingsLimits::MinSimulationTimeScale,
                                         cell::SettingsLimits::MaxSimulationTimeScale);
    ui->cellWidthSpinBox->setRange(cell::SettingsLimits::MinCellWidth, cell::SettingsLimits::MaxCellWidth);
    ui->cellHeightSpinBox->setRange(cell::SettingsLimits::MinCellHeight, cell::SettingsLimits::MaxCellHeight);
    ui->GUIFPSSpinBox->setRange(1, 120);
}