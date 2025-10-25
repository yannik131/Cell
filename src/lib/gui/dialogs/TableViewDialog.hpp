#ifndef C0A1FDE3_4705_4395_96E3_33DE1B8D03BA_HPP
#define C0A1FDE3_4705_4395_96E3_33DE1B8D03BA_HPP

#include "cell/ExceptionWithLocation.hpp"
#include "core/Utility.hpp"
#include "models/AbstractSimulationConfigTableModel.hpp"
#include "ui_TableViewDialog.h"

#include <QDialog>

namespace Ui
{
class TableViewDialog;
}

class SimulationConfigUpdater;

template <typename T> class TableViewDialog : public QDialog
{
public:
    TableViewDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater, AbstractTableModel<T>* model);

    ~TableViewDialog();

protected:
    void showEvent(QShowEvent* showEvent) override;

protected:
    Ui::TableViewDialog* ui;
    SimulationConfigUpdater* simulationConfigUpdater_;
    AbstractTableModel<T>* model_;
};

template <typename T>
inline TableViewDialog<T>::TableViewDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater,
                                           AbstractTableModel<T>* model)
    : QDialog(parent)
    , ui(new Ui::TableViewDialog)
    , simulationConfigUpdater_(simulationConfigUpdater)
    , model_(model)
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked,
            utility::safeSlot(this,
                              [this]()
                              {
                                  if (auto model = dynamic_cast<AbstractSimulationConfigTableModel<T>*>(model_))
                                      model->saveToConfig();
                                  accept();
                              }));
    connect(ui->cancelPushButton, &QPushButton::clicked, [this]() { reject(); });
    connect(ui->addPushButton, &QPushButton::clicked, [this]() { model_->addRow(); });
    connect(ui->clearPushButton, &QPushButton::clicked, [this]() { model_->clearRows(); });

    ui->tableView->setModel(model_);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setEditTriggers(QAbstractItemView::EditTrigger::CurrentChanged |
                                   QAbstractItemView::EditTrigger::SelectedClicked);
}

template <typename T> inline TableViewDialog<T>::~TableViewDialog()
{
    delete ui;
}

template <typename T> inline void TableViewDialog<T>::showEvent(QShowEvent* showEvent)
{
    if (auto model = dynamic_cast<AbstractSimulationConfigTableModel<T>*>(model_))
        model->loadFromConfig();
}

#endif /* C0A1FDE3_4705_4395_96E3_33DE1B8D03BA_HPP */
