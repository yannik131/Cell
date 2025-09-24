#ifndef EBD19D22_0EC6_4D52_8BBA_73271A5FE110_HPP
#define EBD19D22_0EC6_4D52_8BBA_73271A5FE110_HPP

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

#endif /* EBD19D22_0EC6_4D52_8BBA_73271A5FE110_HPP */
