#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "DiscDistributionPreviewTableModel.hpp"
#include "DiscTypeDistributionDialog.hpp"
#include "DiscTypeDistributionTableModel.hpp"
#include "FrameDTO.hpp"
#include "PlotDataSelectionDialog.hpp"
#include "PlotModel.hpp"
#include "ReactionsDialog.hpp"
#include "ReactionsTableModel.hpp"
#include "Simulation.hpp"

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
     * @brief Reinitializes the cell and resets the plot
     */
    void setSimulationWidgetSize();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    QThread* simulationThread_ = nullptr;
    std::unique_ptr<Simulation> simulation_;

    bool initialSizeSet_ = false;
    QTimer resizeTimer_;

    DiscTypeDistributionDialog* discDistributionDialog_;
    ReactionsDialog* reactionsDialog_;
    PlotDataSelectionDialog* plotDataSelectionDialog_;

    PlotModel* plotModel_;
};

#endif /* MAINWINDOW_HPP */
