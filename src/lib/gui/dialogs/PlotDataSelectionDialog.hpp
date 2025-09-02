#ifndef PLOTDATASELECTIONDIALOG_HPP
#define PLOTDATASELECTIONDIALOG_HPP

#include <QDialog>

namespace Ui
{
class PlotDataSelectionDialog;
}

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
    explicit PlotDataSelectionDialog(QWidget* parent = nullptr);

    /**
     * @brief Loads the settings before hiding to make sure they're current when the user reopens the dialog
     */
    void closeEvent(QCloseEvent* event) override;

private:
    /**
     * @brief Saves the selected disc types for plotting in the settings
     */
    void saveSettings();

    /**
     * @brief Loads the selected plot types from the settings to display them
     */
    void loadSettings();

private:
    Ui::PlotDataSelectionDialog* ui;
};

#endif /* PLOTDATASELECTIONDIALOG_HPP */
