#ifndef REACTIONSDIALOG_HPP
#define REACTIONSDIALOG_HPP

#include "ComboBoxDelegate.hpp"
#include "DiscType.hpp"
#include "ReactionsTableModel.hpp"

#include <QDialog>

namespace Ui
{
class ReactionsDialog;
};

/**
 * @brief Dialog where the user can create/delete reactions
 */
class ReactionsDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief Creates the delegates for displaying the reactions and connects callbacks
     */
    explicit ReactionsDialog(QWidget* parent = nullptr);

    /**
     * @brief Loads the settings again so they are displayed correctly next time the dialog is opened
     */
    void closeEvent(QCloseEvent* event) override;

private:
    /**
     * @brief Lets the model add an empty row to the table with the given type
     */
    void requestEmptyRowFromModel(const Reaction::Type& type);

    /**
     * @brief Loads the settings and hides the dialog
     */
    void cancel();

private:
    Ui::ReactionsDialog* ui;
    ReactionsTableModel* reactionsTableModel_;
};

#endif /* REACTIONSDIALOG_HPP */
