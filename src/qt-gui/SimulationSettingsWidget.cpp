#include "SimulationSettingsWidget.hpp"
#include "GlobalSettings.hpp"
#include "SimulationSettings.hpp"

#include <QTimer>
#include <SFML/System/Time.hpp>

SimulationSettingsWidget::SimulationSettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    QTimer::singleShot(
        0,
        [this]()
        {
            // Initialize spin boxes
            fpsSpinBox_ = findChild<QSpinBox*>("fpsSpinBox");
            numberOfDiscsSpinBox_ = findChild<QSpinBox*>("numberOfDiscsSpinBox");
            timeStepSpinBox_ = findChild<QSpinBox*>("timeStepSpinBox");
            collisionUpdateSpinBox_ = findChild<QSpinBox*>("collisionUpdateSpinBox");
            timeScaleSpinBox_ = findChild<QSpinBox*>("timeScaleSpinBox");

            // Set ui elements to default settings
            SimulationSettings defaultSettings;
            fpsSpinBox_->setValue(1000 / defaultSettings.frameTime.asMilliseconds());
            numberOfDiscsSpinBox_->setValue(defaultSettings.numberOfDiscs);
            timeStepSpinBox_->setValue(defaultSettings.simulationTimeStep.asMilliseconds());
            collisionUpdateSpinBox_->setValue(defaultSettings.collisionUpdateTime.asMilliseconds());

            // Connect callback for changed settings
            for (QSpinBox* widget : {fpsSpinBox_, numberOfDiscsSpinBox_, timeStepSpinBox_, collisionUpdateSpinBox_})
            {
                spinBoxList_.append(widget);
                connect(widget, &QSpinBox::valueChanged, this, &SimulationSettingsWidget::onSettingsChanged);
            }
        });
}

void SimulationSettingsWidget::lock()
{
    for (QSpinBox* widget : spinBoxList_)
        widget->setDisabled(true);
}

void SimulationSettingsWidget::unlock()
{
    for (QSpinBox* widget : spinBoxList_)
        widget->setDisabled(false);
}

void SimulationSettingsWidget::onSettingsChanged()
{
    emit settingsChanged();
}
