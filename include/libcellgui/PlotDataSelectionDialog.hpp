#ifndef PLOTDATASELECTIONDIALOG_HPP
#define PLOTDATASELECTIONDIALOG_HPP

#include <QDialog>

namespace Ui
{
class PlotDataSelectionDialog;
}

/**
 * @brief Pretty simple stuff here, just the dialog containing a MultiSelectListWidget for plot data selection
 */
class PlotDataSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotDataSelectionDialog(QWidget* parent = nullptr);

    /**
     * @brief Displays the window names in both widgets and selects them all
     */
    void setAndSelectDiscTypes(const QStringList& discTypes);

    QStringList getSelectedDiscTypeNames() const;

private:
    Ui::PlotDataSelectionDialog* ui;
};

#endif /* PLOTDATASELECTIONDIALOG_HPP */
