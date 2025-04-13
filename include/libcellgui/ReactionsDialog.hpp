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

class ReactionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReactionsDialog(QWidget* parent = nullptr);

    void closeEvent(QCloseEvent* event);
    void showEvent(QShowEvent* event);

private:
    void requestEmptyRowFromModel(const Reaction::Type& type);
    void cancel();
    void updateComboBoxDelegateItems();

private slots:
    void onOK();
    void onCancel();

private:
    Ui::ReactionsDialog* ui;
    ReactionsTableModel* reactionsTableModel_;
    ComboBoxDelegate* comboBoxDelegate_;
};

#endif /* REACTIONSDIALOG_HPP */
