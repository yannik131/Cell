#ifndef DISTRIBUTIONANDREACTIONSDIALOG_HPP
#define DISTRIBUTIONANDREACTIONSDIALOG_HPP

#include "DiscType.hpp"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui
{
class DiscTypesDialog;
};

class DiscTypesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DiscTypesDialog(QWidget* parent = nullptr);

    void closeEvent(QCloseEvent* event);

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

private:
    Ui::DiscTypesDialog* ui;
    QStandardItemModel* discTypesModel_;
};

#endif /* DISTRIBUTIONANDREACTIONSDIALOG_HPP */
