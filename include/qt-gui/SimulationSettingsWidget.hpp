#ifndef SIMULATIONSETTINGSWIDGET_HPP
#define SIMULATIONSETTINGSWIDGET_HPP

#include <QList>
#include <QSpinBox>
#include <QWidget>

class SimulationSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    SimulationSettingsWidget(QWidget* parent = nullptr);

    void lock();
    void unlock();

private:
    void onSettingsChanged();

signals:
    void settingsChanged();

private:
    QSpinBox* fpsSpinBox_;
    QSpinBox* numberOfDiscsSpinBox_;
    QSpinBox* timeStepSpinBox_;
    QSpinBox* collisionUpdateSpinBox_;
    QSpinBox* timeScaleSpinBox_;

    QList<QSpinBox*> spinBoxList_;
};

#endif /* SIMULATIONSETTINGSWIDGET_HPP */
