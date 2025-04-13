#ifndef SIMULATIONCONTROLWIDGET_HPP
#define SIMULATIONCONTROLWIDGET_HPP

#include "DiscDistributionPreviewTableModel.hpp"

#include <QWidget>

namespace Ui
{
class SimulationControlWidget;
}

class SimulationControlWidget : public QWidget
{
public:
    SimulationControlWidget(QObject* parent = nullptr);

signals:
    void simulationStartClicked();
    void simulationStopClicked();
    void simulationResetClicked();
    void editDiscTypesClicked();
    void editReactionsClicked();

private:
    void setRanges();
    void displayGlobalSettings();
    void setCallbacks();
    void toggleStartStopButtonState();
    void reset();

private:
    Ui::SimulationControlWidget* ui;
    DiscDistributionPreviewTableModel* discDistributionPreviewTableModel_;

    bool simulationStarted_ = false;
};

#endif /* SIMULATIONCONTROLWIDGET_HPP */
