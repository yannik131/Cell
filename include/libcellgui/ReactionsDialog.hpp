#ifndef REACTIONSDIALOG_HPP
#define REACTIONSDIALOG_HPP

#include "DiscType.hpp"
#include "Reaction.hpp"

#include <QDialog>
#include <QStandardItemModel>

#include <array>

namespace Ui
{
class ReactionsDialog;
};

class ReactionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReactionsDialog(QWidget* parent = nullptr);

    void closeEvent(QCloseEvent* event);
    void showEvent(QShowEvent* event);

signals:
    void reactionsChanged();

private slots:
    void onOK();
    void onCancel();

    void onAddCombinationReaction();
    void onAddDecompositionReaction();
    void onAddExchangeReaction();
    void onClearReactions();
    void onDeleteReaction();

private:
    void addRowFromReaction(const Reaction& reaction);
    std::vector<Reaction> convertInputsToReactions() const;
    const DiscType& getDefaultDiscType() const;
    void resetTableViewToSettings();

private:
    Ui::ReactionsDialog* ui;
    QStandardItemModel* reactionsModel_;
};

#endif /* REACTIONSDIALOG_HPP */
