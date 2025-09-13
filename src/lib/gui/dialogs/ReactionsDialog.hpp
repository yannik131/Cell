#ifndef REACTIONSDIALOG_HPP
#define REACTIONSDIALOG_HPP

#include <QDialog>

namespace Ui
{
class ReactionsDialog;
};

class ReactionsTableModel;
class AbstractSimulationBuilder;

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
    explicit ReactionsDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

    /**
     * @brief Loads the settings again so they are displayed correctly next time the dialog is opened
     */
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::ReactionsDialog* ui;
    ReactionsTableModel* reactionsTableModel_;
};

#endif /* REACTIONSDIALOG_HPP */
