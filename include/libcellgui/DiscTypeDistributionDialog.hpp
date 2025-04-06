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

    void closeEvent(QCloseEvent* event);
    void setModel(DiscTypeDistributionTableModel* discTypeDistributionTableModel);

signals:
    void discDistributionChanged();

private slots:
    void onOK();
    void onCancel();

    void onAddType();
    void onClearTypes();
    void onDeleteDiscType();

private:
    void validateColorMapping();
    void addTableViewRowFromDiscType(const DiscType& discType, int percentage = 0);
    std::map<DiscType, int> convertInputsToDiscTypeDistribution() const;
    void resetTableViewToSettings();

private:
    Ui::DiscTypeDistributionDialog* ui;
    QStandardItemModel* discTypesModel_;
};

#endif /* DISTRIBUTIONANDREACTIONSDIALOG_HPP */
