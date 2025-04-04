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

signals:
    void selectedDiscTypeNames(const QStringList& discTypeNames);

private slots:
    void emitSelectedDiscTypes();

private:
    void setAndSelectDiscTypes(const QStringList& discTypes);

private:
    Ui::PlotDataSelectionDialog* ui;
};

#endif /* PLOTDATASELECTIONDIALOG_HPP */
