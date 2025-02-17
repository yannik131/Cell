#ifndef DISCDISTRIBUTIONDIALOG_HPP
#define DISCDISTRIBUTIONDIALOG_HPP

#include "DiscType.hpp"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui
{
class DiscDistributionDialog;
};

class DiscDistributionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DiscDistributionDialog(QWidget* parent = nullptr);

    void closeEvent(QCloseEvent* event);

signals:
    void discDistributionChanged();

private slots:
    void onOK();
    void onCancel();
    void onAdd();
    void onClear();
    void onDeleteDiscType();

private:
    void validateColorMapping();
    void addTableViewRowFromDiscType(const DiscType& discType, int percentage = 0);

private:
    Ui::DiscDistributionDialog* ui;
    QStandardItemModel* model_;
};

#endif /* DISCDISTRIBUTIONDIALOG_HPP */
