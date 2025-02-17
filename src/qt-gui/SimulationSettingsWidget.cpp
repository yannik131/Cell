#include "SimulationSettingsWidget.hpp"
#include "ColorMapping.hpp"
#include "GlobalSettings.hpp"

#include <QHeaderView>
#include <QMessageBox>
#include <QScrollBar>
#include <QTimer>
#include <SFML/System/Time.hpp>

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

SimulationSettingsWidget::SimulationSettingsWidget(QWidget* parent)
    : QWidget(parent)
    , model_(new QStandardItemModel(this))
{
    model_->setColumnCount(3);
    model_->setHeaderData(0, Qt::Horizontal, "Type");
    model_->setHeaderData(1, Qt::Horizontal, "Color");
    model_->setHeaderData(2, Qt::Horizontal, "%");

    QTimer::singleShot(0, this, &SimulationSettingsWidget::init);
}

void SimulationSettingsWidget::updateDiscDistributionPreviewTableView()
{
    const auto& discTypeDistribution = GlobalSettings::getSettings().discTypeDistribution_;
    model_->removeRows(0, model_->rowCount());

    for (const auto& [discType, percentage] : discTypeDistribution)
    {
        QList<QStandardItem*> items;

        items.append(new QStandardItem(QString::fromStdString(discType.name_)));
        items.append(new QStandardItem(ColorNameMapping[discType.color_]));
        items.append(new QStandardItem(QString::number(percentage)));

        model_->appendRow(items);
    }
}

void SimulationSettingsWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (!model_ || model_->rowCount() == 0)
        return;

    fitContentIntoTableView();
}

void SimulationSettingsWidget::displayGlobalSettings()
{
    const auto& settings = GlobalSettings::getSettings();
    fpsSpinBox_->setValue(settings.guiFPS_);
    numberOfDiscsSpinBox_->setValue(settings.numberOfDiscs_);
    timeStepSpinBox_->setValue(settings.simulationTimeStep_.asMilliseconds());
    collisionUpdateSpinBox_->setValue(settings.collisionUpdateTime_.asMilliseconds());
    timeScaleDoubleSpinBox_->setValue(settings.simulationTimeScale_);
    frictionDoubleSpinBox_->setValue(settings.frictionCoefficient);
}

void SimulationSettingsWidget::init()
{
    initializeSpinBoxes();
    displayGlobalSettings();
    initializeTableView();
    setCallbacks();
}

void SimulationSettingsWidget::initializeSpinBoxes()
{
    fpsSpinBox_ = findChild<QSpinBox*>("fpsSpinBox");
    numberOfDiscsSpinBox_ = findChild<QSpinBox*>("numberOfDiscsSpinBox");
    timeStepSpinBox_ = findChild<QSpinBox*>("timeStepSpinBox");
    collisionUpdateSpinBox_ = findChild<QSpinBox*>("collisionUpdateSpinBox");
    timeScaleDoubleSpinBox_ = findChild<QDoubleSpinBox*>("timeScaleDoubleSpinBox");
    frictionDoubleSpinBox_ = findChild<QDoubleSpinBox*>("frictionDoubleSpinBox");
    discDistributionPreviewTableView_ = findChild<QTableView*>("discDistributionPreviewTableView");

    fpsSpinBox_->setRange(SettingsLimits::MinGuiFPS, SettingsLimits::MaxGuiFPS);
    numberOfDiscsSpinBox_->setRange(SettingsLimits::MinNumberOfDiscs, SettingsLimits::MaxNumberOfDiscs);
    timeStepSpinBox_->setRange(SettingsLimits::MinSimulationTimeStep.asMilliseconds(),
                               SettingsLimits::MaxSimulationTimeStep.asMilliseconds());
    collisionUpdateSpinBox_->setRange(SettingsLimits::MinCollisionUpdateTime.asMilliseconds(),
                                      SettingsLimits::MaxCollisionUpdateTime.asMilliseconds());
    timeScaleDoubleSpinBox_->setRange(SettingsLimits::MinSimulationTimeScale, SettingsLimits::MaxSimulationTimeScale);
    frictionDoubleSpinBox_->setRange(SettingsLimits::MinFrictionCoefficient, SettingsLimits::MaxFrictionCoefficient);
}

void SimulationSettingsWidget::initializeTableView()
{
    discDistributionPreviewTableView_->setModel(model_);
    fitContentIntoTableView();
    updateDiscDistributionPreviewTableView();
}

void SimulationSettingsWidget::setCallbacks()
{
    // Connect callback for changed settings (after displaying the global settings, otherwise we
    // will trigger a world reset without having set the bounds first)
    connect(fpsSpinBox_, &QSpinBox::valueChanged, this,
            [this](int value) { DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setGuiFPS(value)) });

    connect(numberOfDiscsSpinBox_, &QSpinBox::valueChanged, this,
            [this](int value)
            {
                DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setNumberOfDiscs(value))
                emit settingsChanged();
            });

    connect(timeStepSpinBox_, &QSpinBox::valueChanged, this, [this](int value)
            { DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setSimulationTimeStep(sf::milliseconds(value))) });

    connect(collisionUpdateSpinBox_, &QSpinBox::valueChanged, this, [this](int value)
            { DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setCollisionUpdateTime(sf::milliseconds(value))) });

    connect(timeScaleDoubleSpinBox_, &QDoubleSpinBox::valueChanged, this,
            [this](float value) { DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setSimulationTimeScale(value)) });

    connect(frictionDoubleSpinBox_, &QDoubleSpinBox::valueChanged, this,
            [this](float value) { DISPLAY_EXCEPTION_AND_RETURN(GlobalSettings::get().setFrictionCoefficient(value)) });
}

void SimulationSettingsWidget::fitContentIntoTableView()
{
    int width = discDistributionPreviewTableView_->width();
    discDistributionPreviewTableView_->setColumnWidth(0, width / 3);
    discDistributionPreviewTableView_->setColumnWidth(1, width / 3);
    discDistributionPreviewTableView_->setColumnWidth(2, width / 3);
}

void SimulationSettingsWidget::onSettingsChanged()
{
    emit settingsChanged();
}