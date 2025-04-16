#ifndef DISTRIBUTIONANDREACTIONSDIALOG_HPP
#define DISTRIBUTIONANDREACTIONSDIALOG_HPP

#include "DiscType.hpp"
#include "DiscTypeDistributionTableModel.hpp"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui
{
class DiscTypeDistributionDialog;
};

class DiscTypeDistributionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DiscTypeDistributionDialog(QWidget* parent = nullptr);

private:
    // Only called when the user closes the dialog manually without clicking a custom button
    void closeEvent(QCloseEvent* event);
    void cancel();

signals:
    void dialogClosed();

private:
    Ui::DiscTypeDistributionDialog* ui;
    DiscTypeDistributionTableModel* discTypeDistributionTableModel_;
};

#endif /* DISTRIBUTIONANDREACTIONSDIALOG_HPP */
