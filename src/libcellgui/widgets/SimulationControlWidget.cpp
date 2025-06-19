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
    ui->discDistributionPreviewTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // The order matters here: If we connect the callbacks to the widgets first, setting them to
    // the value from the settings would lead to infinite recursion

    setRanges();
    displayGlobalSettings();
    setCallbacks();
}

void SimulationControlWidget::setRanges()
{
    ui->fpsSpinBox->setRange(GUISettingsLimits::MinGuiFPS, GUISettingsLimits::MaxGuiFPS);
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
    const auto& guiSettings = GlobalGUISettings::getGUISettings();
    ui->fpsSpinBox->setValue(guiSettings.guiFPS_);

    const auto& settings = cell::GlobalSettings::getSettings();
    ui->numberOfDiscsSpinBox->setValue(settings.numberOfDiscs_);
    ui->timeStepSpinBox->setValue(static_cast<int>(settings.simulationTimeStep_.asMicroseconds()));
    ui->timeScaleDoubleSpinBox->setValue(settings.simulationTimeScale_);
    ui->cellWidthSpinBox->setValue(settings.cellWidth_);
    ui->cellHeightSpinBox->setValue(settings.cellHeight_);
}

void SimulationControlWidget::setCallbacks()
{
    // Connect callback for changed settings (after displaying the global settings, otherwise we
    // will trigger a world reset without having set the bounds first)
    connect(ui->fpsSpinBox, &QSpinBox::valueChanged,
            [this](int value) { tryExecuteWithExceptionHandling([=] { GlobalGUISettings::get().setGuiFPS(value); }); });

    connect(ui->numberOfDiscsSpinBox, &QSpinBox::valueChanged,
            [this](int value)
            {
                tryExecuteWithExceptionHandling(
                    [value, this]
                    {
                        cell::GlobalSettings::get().setNumberOfDiscs(value);
                        emit simulationResetTriggered();
                    });
            });

    connect(ui->timeStepSpinBox, &QSpinBox::valueChanged,
            [this](int value)
            {
                tryExecuteWithExceptionHandling(
                    [value] { cell::GlobalSettings::get().setSimulationTimeStep(sf::microseconds(value)); });
            });

    connect(
        ui->timeScaleDoubleSpinBox, &QDoubleSpinBox::valueChanged, [this](float value)
        { tryExecuteWithExceptionHandling([value] { cell::GlobalSettings::get().setSimulationTimeScale(value); }); });
    connect(ui->cellWidthSpinBox, &QSpinBox::valueChanged,
            [this](int value)
            {
                tryExecuteWithExceptionHandling(
                    [value, this]
                    {
                        cell::GlobalSettings::get().setCellSize(value, ui->cellHeightSpinBox->value());
                        emit simulationResetTriggered();
                    });
            });
    connect(ui->cellHeightSpinBox, &QSpinBox::valueChanged,
            [this](int value)
            {
                tryExecuteWithExceptionHandling(
                    [value, this]
                    {
                        cell::GlobalSettings::get().setCellSize(ui->cellWidthSpinBox->value(), value);
                        emit simulationResetTriggered();
                    });
            });

    connect(ui->editDiscTypesPushButton, &QPushButton::clicked, [this]() { emit editDiscTypesClicked(); });
    connect(ui->editReactionsPushButton, &QPushButton::clicked, [this]() { emit editReactionsClicked(); });

    connect(ui->startStopButton, &QPushButton::clicked, this, &SimulationControlWidget::toggleStartStopButtonState);
    connect(ui->reinitializeButton, &QPushButton::clicked, this, &SimulationControlWidget::reset);
    connect(ui->fitIntoViewButton, &QPushButton::clicked, [this]() { emit fitIntoViewRequested(); });

    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::numberOfDiscsChanged,
            [this]() { ui->numberOfDiscsSpinBox->setValue(cell::GlobalSettings::getSettings().numberOfDiscs_); });
    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::simulationTimeScaleChanged, [this]()
            { ui->timeScaleDoubleSpinBox->setValue(cell::GlobalSettings::getSettings().simulationTimeScale_); });
    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::simulationTimeStepChanged,
            [this]()
            {
                ui->timeStepSpinBox->setValue(
                    static_cast<int>(cell::GlobalSettings::getSettings().simulationTimeStep_.asMicroseconds()));
            });
    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::cellSizeChanged,
            [this]()
            {
                ui->cellWidthSpinBox->setValue(cell::GlobalSettings::getSettings().cellWidth_);
                ui->cellHeightSpinBox->setValue(cell::GlobalSettings::getSettings().cellHeight_);
            });
}

void SimulationControlWidget::toggleStartStopButtonState()
{
    if (cell::GlobalSettings::getSettings().discTypeDistribution_.empty())
    {
        QMessageBox::information(this, "Can't start simulation",
                                 "Can't start simulation with an empty disc type distribution");
        return;
    }
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
