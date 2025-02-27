#ifndef DISTRIBUTIONANDREACTIONSDIALOG_HPP
#define DISTRIBUTIONANDREACTIONSDIALOG_HPP

#include "DiscType.hpp"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui
{
class DistributionAndReactionsDialog;
};

class DistributionAndReactionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DistributionAndReactionsDialog(QWidget* parent = nullptr);

    void closeEvent(QCloseEvent* event);

signals:
    void discDistributionChanged();

private slots:
    void onOK();
    void onCancel();

    void onAddType();
    void onClearTypes();
    void onDeleteDiscType();

    void onAddReaction();
    void onClearReactions();
    void onDeleteReaction();

private:
    void validateColorMapping();
    void addTableViewRowFromDiscType(const DiscType& discType, int percentage = 0);
    void setModelHeaderData(QStandardItemModel* model, const QStringList& headers);

private:
    Ui::DistributionAndReactionsDialog* ui;
    QStandardItemModel* discDistributionModel_;
    QStandardItemModel* reactionsModel_;
};

#endif /* DISTRIBUTIONANDREACTIONSDIALOG_HPP */
