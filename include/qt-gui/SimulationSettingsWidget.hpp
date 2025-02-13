#ifndef SIMULATIONSETTINGSWIDGET_HPP
#define SIMULATIONSETTINGSWIDGET_HPP

#include "SimulationSettings.hpp"

#include <QWidget>
#include <QSpinBox>
#include <QList>

class SimulationSettingsWidget : public  QWidget
{
    Q_OBJECT
public:
    SimulationSettingsWidget(QWidget* parent = nullptr);

    void lock();
    void unlock();

private:
    void onSettingsChanged();
    SimulationSettings getSimulationSettings() const;

signals:
    void settingsChanged(const SimulationSettings& settings);

private:
    QSpinBox* fpsSpinBox_;
    QSpinBox* numberOfDiscsSpinBox_;
    QSpinBox* timeStepSpinBox_;
    QSpinBox* collisionUpdateSpinBox_;

    QList<QSpinBox*> spinBoxList_;
};

#endif /* SIMULATIONSETTINGSWIDGET_HPP */
