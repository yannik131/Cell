#include "SimulationControlWidget.hpp"
#include "GlobalSettings.hpp"
#include "ui_SimulationControlWidget.h"

#define DISPLAY_EXCEPTION_AND_RETURN(statement)                                                                        \
    try                                                                                                                \
    {                                                                                                                  \
        statement;                                                                                                     \
    }                                                                                                                  \
    catch (const std::exception& e)                                                                                    \
    {                                                                                                                  \
        displayGlobalSettings();                                                                                       \
        QMessageBox::critical(this, "Fehler", e.what());                                                               \
        return;                                                                                                        \
    }

SimulationControlWidget::SimulationControlWidget(QObject* parent)
    : QWidget(parent)
    , ui(new Ui::SimulationControlWidget(this))
    , discDistributionPreviewTableModel_(new DiscDistributionPreviewTableModel(this))
{
    discDistributionPreviewTableModel_->loadSettings();

    setRanges();
    displayGlobalSettings();
    setCallbacks();
}

void SimulationControlWidget::setRanges()
{
    ui->fpsSpinBox->setRange(SettingsLimits::MinGuiFPS, SettingsLimits::MaxGuiFPS);
    ui->numberOfDiscsSpinBox->setRange(SettingsLimits::MinNumberOfDiscs, SettingsLimits::MaxNumberOfDiscs);
    ui->timeStepSpinBox->setRange(SettingsLimits::MinSimulationTimeStep.asMilliseconds(),
                                  SettingsLimits::MaxSimulationTimeStep.asMilliseconds());
    ui->plotTimeIntervalSpinBox->setRange(SettingsLimits::MinPlotTimeInterval.asMilliseconds(),
                                          SettingsLimits::MaxPlotTimeInterval.asMilliseconds());
    ui->timeScaleDoubleSpinBox->setRange(SettingsLimits::MinSimulationTimeScale,
                                         SettingsLimits::MaxSimulationTimeScale);
    ui->frictionDoubleSpinBox->setRange(SettingsLimits::MinFrictionCoefficient, SettingsLimits::MaxFrictionCoefficient);
}

void SimulationControlWidget::displayGlobalSettings()
{
    const auto& settings = GlobalSettings::getSettings();
    ui->fpsSpinBox->setValue(settings.guiFPS_);
    ui->numberOfDiscsSpinBox->setValue(settings.numberOfDiscs_);
    ui->timeStepSpinBox->setValue(settings.simulationTimeStep_.asMilliseconds());
    ui->plotTimeIntervalSpinBox->setValue(settings.plotTimeInterval_.asMilliseconds());
    ui->timeScaleDoubleSpinBox->setValue(settings.simulationTimeScale_);
    ui->frictionDoubleSpinBox->setValue(settings.frictionCoefficient);
}

void SimulationControlWidget::setCallbacks()
{
    // Connect callback for changed settings (after displaying the global settings, otherwise we
    // will trigger a world reset without having set the bounds first)
    connect(ui->fpsSpinBox, &QSpinBox::valueChanged, this,
            [this](int value) { DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setGuiFPS(value)) });

    connect(ui->numberOfDiscsSpinBox, &QSpinBox::valueChanged, this,
            [this](int value)
            {
                DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setNumberOfDiscs(value))
                emit settingsChanged();
            });

    connect(ui->timeStepSpinBox, &QSpinBox::valueChanged, this, [this](int value)
            { DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setSimulationTimeStep(sf::milliseconds(value))) });

    connect(ui->plotTimeIntervalSpinBox, &QSpinBox::valueChanged, this, [this](int value)
            { DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setPlotTimeInterval(sf::milliseconds(value))) });

    connect(ui->timeScaleDoubleSpinBox, &QDoubleSpinBox::valueChanged, this,
            [this](float value) { DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setSimulationTimeScale(value)) });

    connect(ui->frictionDoubleSpinBox, &QDoubleSpinBox::valueChanged, this,
            [this](float value) { DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setFrictionCoefficient(value)) });
}
