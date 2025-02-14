#include "SimulationSettingsWidget.hpp"
#include "SimulationSettings.hpp"

#include <SFML/System/Time.hpp>
#include <QTimer>

SimulationSettingsWidget::SimulationSettingsWidget(QWidget* parent) : QWidget(parent)
{
    QTimer::singleShot(0, [this]() {
        // Initialize spin boxes
        fpsSpinBox_ = findChild<QSpinBox*>("fpsSpinBox");
        numberOfDiscsSpinBox_ = findChild<QSpinBox*>("numberOfDiscsSpinBox");
        timeStepSpinBox_ = findChild<QSpinBox*>("timeStepSpinBox");
        collisionUpdateSpinBox_ = findChild<QSpinBox*>("collisionUpdateSpinBox");
    
        // Set ui elements to default settings
        SimulationSettings defaultSettings;
        fpsSpinBox_->setValue(1000 / defaultSettings.frameTime.asMilliseconds());
        numberOfDiscsSpinBox_->setValue(defaultSettings.numberOfDiscs);
        timeStepSpinBox_->setValue(defaultSettings.simulationTimeStep.asMilliseconds());
        collisionUpdateSpinBox_->setValue(defaultSettings.collisionUpdateTime.asMilliseconds());

        // Connect callback for changed settings
        for(QSpinBox* widget : {fpsSpinBox_, numberOfDiscsSpinBox_, timeStepSpinBox_, collisionUpdateSpinBox_}) {
            spinBoxList_.append(widget);
            connect(widget, &QSpinBox::valueChanged, this, &SimulationSettingsWidget::onSettingsChanged);
        }
    });
}

void SimulationSettingsWidget::lock()
{
    for(QSpinBox* widget : spinBoxList_)
        widget->setDisabled(true);
}

void SimulationSettingsWidget::unlock()
{
    for(QSpinBox* widget : spinBoxList_)
        widget->setDisabled(false);
}

void SimulationSettingsWidget::onSettingsChanged()
{
    const auto& settings = getSimulationSettings();
    emit settingsChanged(settings);
}

SimulationSettings SimulationSettingsWidget::getSimulationSettings() const
{
    SimulationSettings settings;
    settings.frameTime = sf::milliseconds(1000 / fpsSpinBox_->value());
    settings.numberOfDiscs = numberOfDiscsSpinBox_->value();
    settings.simulationTimeStep = sf::milliseconds(timeStepSpinBox_->value());
    settings.collisionUpdateTime = sf::milliseconds(collisionUpdateSpinBox_->value());

    return settings;
}
