#ifndef BFD742A1_095D_472C_9947_A6D648D19B4D_HPP
#define BFD742A1_095D_472C_9947_A6D648D19B4D_HPP

#include "core/Types.hpp"

#include <QMessageBox>
#include <QWidget>

namespace Ui
{
class SimulationControlWidget;
}

/**
 * @brief Widget containing all the widgets with which simulation settings can be changed by the user (number of discs,
 * time step, etc.)
 */
class SimulationControlWidget : public QWidget
{
    Q_OBJECT
public:
    SimulationControlWidget(QWidget* parent = nullptr);

    void updateWidgets(SimulationRunning simulationRunning);

private:
    void setWidgetsEnabled(bool value);

signals:
    void simulationStartClicked();
    void simulationStopClicked();
    void simulationResetTriggered();
    void editDiscTypesClicked();
    void editDiscsClicked();
    void editMembraneTypesClicked();
    void editMembranesClicked();
    void editReactionsClicked();
    void editSetupClicked();
    void fitIntoViewRequested();

private:
    /**
     * @brief Changes the start/stop button text depending on wether the simulation is running or nto
     */
    void toggleStartStopButtonState();

    /**
     * @brief Callback for the reset button: Emit the signal for resetting the simulation and update the button text to
     * "start"
     */
    void reset();

private:
    Ui::SimulationControlWidget* ui;
};

#endif /* BFD742A1_095D_472C_9947_A6D648D19B4D_HPP */
