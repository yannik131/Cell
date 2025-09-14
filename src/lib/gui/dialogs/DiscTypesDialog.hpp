#ifndef DISCTYPESDIALOG_HPP
#define DISCTYPESDIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>

namespace Ui
{
class DiscTypesDialog;
};

class DiscTypesTableModel;
class AbstractSimulationBuilder;

/**
 * @brief Dialog where the user can create/delete disc types and specify how many of them should be created initially
 */
class DiscTypesDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief Creates the delegates and sets them in the table view. Also connects all necessary callbacks
     */
    explicit DiscTypesDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

private:
    void showEvent(QShowEvent* event) override;

private:
    Ui::DiscTypesDialog* ui;
    DiscTypesTableModel* discTypesTableModel_;
};

#endif /* DISCTYPESDIALOG_HPP */
