#ifndef SIMULATIONCONTROLWIDGET_HPP
#define SIMULATIONCONTROLWIDGET_HPP

#include "models/DiscDistributionPreviewTableModel.hpp"

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

signals:
    void simulationStartClicked();
    void simulationStopClicked();
    void simulationResetTriggered();
    void editDiscTypesClicked();
    void editReactionsClicked();
    void fitIntoViewRequested();

private:
    /**
     * @brief Reads the limits for the different settings from the settings and adjusts the widgets accordingly
     */
    void setRanges();

    /**
     * @brief Sets the values of all settings widgets to the one currently in the settings
     */
    void displayGlobalSettings();

    /**
     * @brief Connects the callbacks to the widgets, making sure that changing values in the GUI will actually change
     * the settings
     */
    void setCallbacks();

    /**
     * @brief Changes the start/stop button text depending on wether the simulation is running or nto
     */
    void toggleStartStopButtonState();

    /**
     * @brief Callback for the reset button: Emit the signal for resetting the simulation and update the button text to
     * "start"
     */
    void reset();

    /**
     * @brief Theoretically, after setting the limits for the widgets based on the settings, GlobalSettings should never
     * throw when setting a value with one of those widgets because 1. the main window will disable this widget when the
     * simulation runs, so changing settings during simulation shouldn't be possible and 2. the values will always be in
     * the correct range. Anyways, if for whatever reason GlobalSettings does throw after setting a setting, we need to
     * catch that, which this function is for.
     */
    template <typename Func> void tryExecuteWithExceptionHandling(Func&& func)
    {
        try
        {
            std::forward<Func>(func)();
        }
        catch (const std::exception& e)
        {
            displayGlobalSettings();
            QMessageBox::critical(this, "Fehler", e.what());
        }
    }

private:
    Ui::SimulationControlWidget* ui;
    DiscDistributionPreviewTableModel* discDistributionPreviewTableModel_;

    bool simulationStarted_ = false;
};

#endif /* SIMULATIONCONTROLWIDGET_HPP */
