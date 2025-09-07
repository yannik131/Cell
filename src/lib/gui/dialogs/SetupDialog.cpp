#include "dialogs/SetupDialog.hpp"
#include "SetupDialog.hpp"
#include "cell/Settings.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
#include "models/DiscTableModel.hpp"
#include "models/DiscTypeDistributionTableModel.hpp"
#include "ui_SetupDialog.h"

SetupDialog::SetupDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QDialog(parent)
    , ui(new Ui::SetupDialog)
    , discTypeDistributionTableModel_(new DiscTypeDistributionTableModel(this, abstractSimulationBuilder))
    , discTableModel_(new DiscTableModel(this, abstractSimulationBuilder))
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

    auto updateWidgets = [this](bool on)
    {
        setupModel_->setUseDistribution(on);
        ui->distributionWidget->setEnabled(on);
        ui->manualWidget->setDisabled(on);
    };
    connect(ui->useDistributionRadioButton, &QRadioButton::toggled, this, updateWidgets);
    updateWidgets(ui->useDistributionRadioButton->isChecked());

    connect(ui->addDistributionEntryPushButton, &QPushButton::clicked, discTypeDistributionTableModel_,
            utility::safeSlot(this, [this]() { discTypeDistributionTableModel_->addRow(); }));
    connect(ui->clearDistributionPushButton, &QPushButton::clicked, discTypeDistributionTableModel_,
            &DiscTypeDistributionTableModel::clearRows);

    connect(ui->addDiscPushButton, &QPushButton::clicked, discTableModel_,
            utility::safeSlot(this, [this]() { discTableModel_->addRow(); }));
    connect(ui->clearDiscsPushButton, &QPushButton::clicked, discTableModel_, &DiscTableModel::clearRows);

    connect(ui->numberOfDiscsSpinBox, &QSpinBox::valueChanged, setupModel_, &SetupModel::setNumberOfDiscs);
    connect(ui->timeStepSpinBox, &QSpinBox::valueChanged, setupModel_, &SetupModel::setTimeStepUs);
    connect(ui->timeScaleDoubleSpinBox, &QDoubleSpinBox::valueChanged, setupModel_, &SetupModel::setTimeScale);
    connect(ui->cellWidthSpinBox, &QSpinBox::valueChanged, setupModel_, &SetupModel::setCellWidth);
    connect(ui->cellHeightSpinBox, &QSpinBox::valueChanged, setupModel_, &SetupModel::setCellHeight);
    connect(ui->GUIFPSSpinBox, &QSpinBox::valueChanged, setupModel_, &SetupModel::setGUIFPS);

    ui->numberOfDiscsSpinBox->setRange(cell::SettingsLimits::MinNumberOfDiscs, cell::SettingsLimits::MaxNumberOfDiscs);
    ui->timeStepSpinBox->setRange(static_cast<int>(cell::SettingsLimits::MinSimulationTimeStep.asMicroseconds()),
                                  static_cast<int>(cell::SettingsLimits::MaxSimulationTimeStep.asMicroseconds()));
    ui->timeScaleDoubleSpinBox->setRange(cell::SettingsLimits::MinSimulationTimeScale,
                                         cell::SettingsLimits::MaxSimulationTimeScale);
    ui->cellWidthSpinBox->setRange(cell::SettingsLimits::MinCellWidth, cell::SettingsLimits::MaxCellWidth);
    ui->cellHeightSpinBox->setRange(cell::SettingsLimits::MinCellHeight, cell::SettingsLimits::MaxCellHeight);
    ui->GUIFPSSpinBox->setRange(1, 120);

    insertDiscTypeComboboxIntoView(ui->discTypeDistributionTableView, abstractSimulationBuilder, 0);
    insertDiscTypeComboboxIntoView(ui->discsTableView, abstractSimulationBuilder, 0);

    insertProbabilitySpinBoxIntoView(ui->discTypeDistributionTableView, 1);

    insertDeleteButtonIntoView(this, discTypeDistributionTableModel_, ui->discTypeDistributionTableView, 2);
    insertDeleteButtonIntoView(this, discTableModel_, ui->discsTableView, 5);

    ui->discTypeDistributionTableView->setModel(discTypeDistributionTableModel_);
    ui->discsTableView->setModel(discTableModel_);

    ui->discTypeDistributionTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->discsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    displayCurrentSetup();
}
void SetupDialog::displayCurrentSetup()
{
    const auto& setup = setupModel_->getSetup();
    ui->numberOfDiscsSpinBox->setValue(setup.discCount);
    ui->timeStepSpinBox->setValue(static_cast<int>(std::round(setup.simulationTimeStep * 1e6)));
    ui->timeScaleDoubleSpinBox->setValue(setup.simulationTimeScale);
    ui->cellWidthSpinBox->setValue(static_cast<int>(std::round(setup.cellWidth)));
    ui->cellHeightSpinBox->setValue(static_cast<int>(std::round(setup.cellHeight)));
}