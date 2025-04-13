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

private:
    void setRanges();
    void displayGlobalSettings();
    void setCallbacks();

private:
    Ui::SimulationControlWidget* ui;
    DiscDistributionPreviewTableModel* discDistributionPreviewTableModel_;
};

#endif /* SIMULATIONCONTROLWIDGET_HPP */
