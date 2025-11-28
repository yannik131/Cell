#include "dialogs/ReactionsDialog.hpp"
#include "core/SafeCast.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"

#include <QMenu>

ReactionsDialog::ReactionsDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new ReactionsTableModel(nullptr, simulationConfigUpdater))
{
    connect(static_cast<ReactionsTableModel*>(model_), &ReactionsTableModel::newRowRequested,
            [this]()
            {
                using Type = cell::Reaction::Type;
                auto model = safeCast<ReactionsTableModel*>(model_);

                QMenu menu;
                menu.addAction("Transformation",
                               utility::safeSlot(this, [model]() { model->addRow(Type::Transformation); }));
                menu.addAction("Decomposition",
                               utility::safeSlot(this, [model]() { model->addRow(Type::Decomposition); }));
                menu.addAction("Combination", utility::safeSlot(this, [model]() { model->addRow(Type::Combination); }));
                menu.addAction("Exchange", utility::safeSlot(this, [model]() { model->addRow(Type::Exchange); }));

                menu.exec(QCursor::pos());
            });

    insertDiscTypeComboBoxIntoView(ui->tableView, simulationConfigUpdater, 0, 2, 4, 6);
    insertProbabilitySpinBoxIntoView(ui->tableView, Column{7});
    insertDeleteButtonIntoView(model_, ui->tableView, Column{8});
}
