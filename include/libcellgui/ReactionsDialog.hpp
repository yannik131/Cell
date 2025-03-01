#ifndef REACTIONSDIALOG_HPP
#define REACTIONSDIALOG_HPP

#include "DiscType.hpp"

#include <QDialog>
#include <QStandardItemModel>

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

signals:
    void reactionsChanged();

private slots:
    void onOK();
    void onCancel();

    void onAddReaction();
    void onClearReactions();
    void onDeleteReaction();

private:
    Ui::ReactionsDialog* ui;
    QStandardItemModel* reactionsModel_;
};

#endif /* REACTIONSDIALOG_HPP */
