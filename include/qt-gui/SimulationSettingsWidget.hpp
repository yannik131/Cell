#ifndef SIMULATIONSETTINGSWIDGET_HPP
#define SIMULATIONSETTINGSWIDGET_HPP

#include <QDoubleSpinBox>
#include <QList>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QTableView>
#include <QWidget>

class SimulationSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    SimulationSettingsWidget(QWidget* parent = nullptr);

    void updateDiscDistributionPreviewTableView();

private:
    void displayGlobalSettings();
    void init();

private slots:
    void onSettingsChanged();

signals:
    void settingsChanged();

private:
    QSpinBox* fpsSpinBox_;
    QSpinBox* numberOfDiscsSpinBox_;
    QSpinBox* timeStepSpinBox_;
    QSpinBox* collisionUpdateSpinBox_;
    QDoubleSpinBox* timeScaleDoubleSpinBox_;
    QTableView* discDistributionPreviewTableView_;

    QStandardItemModel* model_;
};

#endif /* SIMULATIONSETTINGSWIDGET_HPP */
