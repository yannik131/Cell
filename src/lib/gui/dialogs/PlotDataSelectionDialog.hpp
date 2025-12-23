#ifndef F3F07B52_942B_4DBE_A7F3_81D8DD5FD11C_HPP
#define F3F07B52_942B_4DBE_A7F3_81D8DD5FD11C_HPP

#include <QDialog>

namespace Ui
{
class PlotDataSelectionDialog;
}

class SimulationConfigUpdater;
class PlotModel;

/**
 * @brief Pretty simple stuff here, just the dialog containing a MultiSelectListWidget for plot data selection:
 * Selecting the disc types whose information should be plotted
 */
class PlotDataSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief Loads the settings and connects the callbacks
     */
    PlotDataSelectionDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater, PlotModel* plotModel);
    ~PlotDataSelectionDialog() override;

    void showEvent(QShowEvent* event) override;

private:
    std::unique_ptr<Ui::PlotDataSelectionDialog> ui;
    SimulationConfigUpdater* simulationConfigUpdater_;
    PlotModel* plotModel_;
};

#endif /* F3F07B52_942B_4DBE_A7F3_81D8DD5FD11C_HPP */
