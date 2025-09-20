#ifndef PLOTDATASELECTIONDIALOG_HPP
#define PLOTDATASELECTIONDIALOG_HPP

#include <QDialog>

namespace Ui
{
class PlotDataSelectionDialog;
}

class AbstractSimulationBuilder;
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
    explicit PlotDataSelectionDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder,
                                     PlotModel* plotModel);

    void showEvent(QShowEvent* event) override;

private:
    Ui::PlotDataSelectionDialog* ui;
    AbstractSimulationBuilder* abstractSimulationBuilder_;
    PlotModel* plotModel_;
};

#endif /* PLOTDATASELECTIONDIALOG_HPP */
