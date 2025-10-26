#include "dialogs/MembraneTypesDialog.hpp"
#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
#include "dialogs/DiscTypeDistributionDialog.hpp"
#include "dialogs/PermeabilityDialog.hpp"
#include "models/MembraneTypesTableModel.hpp"

MembraneTypesDialog::MembraneTypesDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new MembraneTypesTableModel(this, simulationConfigUpdater))
    , permeabilityDialog_(new PermeabilityDialog(this, simulationConfigUpdater))
    , discTypeDistributionDialog_(new DiscTypeDistributionDialog(this, simulationConfigUpdater))
{
    auto* radiusSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(ui->tableView);
    connect(radiusSpinBoxDelegate, &SpinBoxDelegate<QDoubleSpinBox>::editorCreated,
            [](QWidget* spinBox)
            {
                safeCast<QDoubleSpinBox*>(spinBox)->setRange(cell::MembraneTypeLimits::MinRadius,
                                                             cell::MembraneTypeLimits::MaxRadius);
            });

    auto* colorComboBoxDelegate = new ComboBoxDelegate(ui->tableView);
    connect(colorComboBoxDelegate, &ComboBoxDelegate::editorCreated,
            [](QComboBox* comboBox) { comboBox->addItems(getSupportedDiscColorNames()); });

    auto* editPermeabilityPushButtonDelegate = new ButtonDelegate(ui->tableView, "Edit");

    connect(editPermeabilityPushButtonDelegate, &ButtonDelegate::buttonClicked, this,
            [&](int row)
            {
                selectedRowForPermeabilityEditing_ = row;
                auto permeabilityMap = model_->getRows()[row].permeabilityMap;
                permeabilityDialog_->setPermeabilityMap(permeabilityMap);
                permeabilityDialog_->show();
            });
    connect(permeabilityDialog_, &QDialog::accepted,
            [this]()
            {
                auto permeabilityMap = permeabilityDialog_->getPermeabilityMap();
                auto rows = model_->getRows();
                rows[selectedRowForPermeabilityEditing_].permeabilityMap = std::move(permeabilityMap);
                model_->setRows(std::move(rows));
            });

    auto* editDistributionPushButtonDelegate = new ButtonDelegate(ui->tableView, "Edit");
    connect(editDistributionPushButtonDelegate, &ButtonDelegate::buttonClicked,
            [this](int row)
            {
                selectedRowForDistributionEditing_ = row;
                auto distribution = model_->getRows()[row].discTypeDistribution;
                discTypeDistributionDialog_->setDiscTypeDistribution(distribution);
                discTypeDistributionDialog_->show();
            });
    connect(discTypeDistributionDialog_, &QDialog::accepted,
            [this]()
            {
                auto distribution = discTypeDistributionDialog_->getDiscTypeDistribution();
                auto rows = model_->getRows();
                rows[selectedRowForDistributionEditing_].discTypeDistribution = std::move(distribution);
                model_->setRows(std::move(rows));
            });

    ui->tableView->setItemDelegateForColumn(1, radiusSpinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(2, colorComboBoxDelegate);
    ui->tableView->setItemDelegateForColumn(3, editPermeabilityPushButtonDelegate);
    ui->tableView->setItemDelegateForColumn(4, editDistributionPushButtonDelegate);
    insertDeleteButtonIntoView(model_, ui->tableView, 5);
}
