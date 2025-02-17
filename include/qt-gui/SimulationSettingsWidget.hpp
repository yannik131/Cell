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

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void displayGlobalSettings();
    void init();
    void initializeSpinBoxes();
    void initializeTableView();
    void setCallbacks();
    void fitContentIntoTableView();

private slots:
    void onSettingsChanged();

signals:
    /**
     * @brief Causes the world to reinitialize
     */
    void settingsChanged();

private:
    QSpinBox* fpsSpinBox_;
    QSpinBox* numberOfDiscsSpinBox_;
    QSpinBox* timeStepSpinBox_;
    QSpinBox* collisionUpdateSpinBox_;
    QDoubleSpinBox* timeScaleDoubleSpinBox_;
    QDoubleSpinBox* frictionDoubleSpinBox_;
    QTableView* discDistributionPreviewTableView_;

    QStandardItemModel* model_;
};

#endif /* SIMULATIONSETTINGSWIDGET_HPP */
