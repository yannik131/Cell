#ifndef REACTIONSDIALOG_HPP
#define REACTIONSDIALOG_HPP

#include "DiscType.hpp"

#include <QDialog>
#include <QStandardItemModel>

#include <array>

namespace Ui
{
class ReactionsDialog;
};

enum class ReactionType
{
    Combination,
    Decomposition
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
    void onClearReactions();
    void onDeleteReaction();

private:
    void addRowFromCombinationReaction(const std::pair<DiscType, DiscType>& educts,
                                       const std::vector<std::pair<DiscType, float>>& products);
    void addRowFromDecompositionReaction(const DiscType& educt,
                                         const std::vector<std::pair<std::pair<DiscType, DiscType>, float>>& products);
    void addReactionRow(std::array<QString, 4> selectedDiscTypes, float probability);
    void resetTableViewToSettings();

    std::map<std::pair<DiscType, DiscType>, std::vector<std::pair<DiscType, float>>>
    convertInputsToCombinationReactions() const;
    std::map<DiscType, std::vector<std::pair<std::pair<DiscType, DiscType>, float>>>
    convertInputsToDecompositionReactions() const;
    ReactionType getRowReactionType(int index) const;

private:
    Ui::ReactionsDialog* ui;
    QStandardItemModel* reactionsModel_;
};

#endif /* REACTIONSDIALOG_HPP */
