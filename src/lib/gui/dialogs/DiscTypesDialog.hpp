#ifndef DISCTYPESDIALOG_HPP
#define DISCTYPESDIALOG_HPP

#include "models/DiscTypesTableModel.hpp"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui
{
class DiscTypesDialog;
};

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
    explicit DiscTypesDialog(QWidget* parent = nullptr);

    DiscTypesTableModel* getModel();

private:
    /**
     * @brief Reloads the settings. This is necessary for when the user changes some stuff and then cancels. Opening the
     * dialog again would show the discarded changes if we don't load the settings again
     */
    void closeEvent(QCloseEvent* event) override;

    /**
     * @brief Hides the dialog and loads the settings again (see closeEvent)
     */
    void cancel();

private:
    Ui::DiscTypesDialog* ui;
    DiscTypesTableModel* discTypesTableModel_;
};

#endif /* DISCTYPESDIALOG_HPP */
