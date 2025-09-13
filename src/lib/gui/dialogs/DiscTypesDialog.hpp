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
    /**
     * @brief Reloads the settings. This is necessary for when the user changes some stuff and then cancels. Opening the
     * dialog again would show the discarded changes if we don't load the settings again
     */
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::DiscTypesDialog* ui;
    DiscTypesTableModel* discTypesTableModel_;
};

#endif /* DISCTYPESDIALOG_HPP */
