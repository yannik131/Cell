#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "core/Simulation.hpp"
#include "dialogs/DiscTypeDistributionDialog.hpp"
#include "dialogs/PlotDataSelectionDialog.hpp"
#include "dialogs/ReactionsDialog.hpp"
#include "models/PlotModel.hpp"

#include <QMainWindow>
#include <QThread>
#include <QTimer>

namespace Ui
{
class MainWindow;
}

/**
 * @brief Contains the simulation thread to start/stop the simulation and connects various callbacks from the simulation
 * to the appropriate widgets
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief Connects the callbacks to the other widgets. Also sets a resize timer so every resize event will not
     * immediately cause a reset of the world (will be removed later)
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Necessary for unique_ptr, since MainWindow is incomplete type here
     */
    ~MainWindow() override;

protected:
    /**
     * @brief A resize of the main window resets the world and the plot right now, see
     * https://github.com/yannik131/Cell/issues/25
     */
    void resizeEvent(QResizeEvent* event) override;

private:
    /**
     * @brief Starts the simulation in a separate thread and waits for it to exit before moving it back to the main
     * thread
     */
    void startSimulation();

    /**
     * @brief Requests an interruption of the simulation thread (will be caught)
     */
    void stopSimulation();

    /**
     * @brief Reinitializes the cell and stops the simulation
     */
    void resetSimulation();

    /**
     * @brief Opens a file dialog and asks the user where to save the settings
     */
    void saveSettingsAsJson();

    /**
     * @brief Opens a file dialog so the user can select a json file to load the settings from
     */
    void loadSettingsFromJson();

    /**
     * @brief Looks for a file called defaultSettings.json in the cwd and loads it if it's there
     */
    void loadDefaultSettings();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    QThread* simulationThread_ = nullptr;
    std::unique_ptr<Simulation> simulation_;

    DiscTypeDistributionDialog* discDistributionDialog_;
    ReactionsDialog* reactionsDialog_;
    PlotDataSelectionDialog* plotDataSelectionDialog_;

    PlotModel* plotModel_;

    QTimer resizeTimer_;
};

#endif /* MAINWINDOW_HPP */
