#include "models/MembraneTypesTableModel.hpp"
#include "MembraneTypesTableModel.hpp"
#include "core/ColorMapping.hpp"
#include "core/SimulationConfigUpdater.hpp"

// TODO DRY violation with DiscTypesTableModel

MembraneTypesTableModel::MembraneTypesTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, {{"Name", "Radius", "Color", "Permeabilities", "Distribution", "Disc count", "Delete"}},
           simulationConfigUpdater)
{
}

void MembraneTypesTableModel::removeRow(int row)
{
    Base::removeRow(row);
    membraneColors_.erase(membraneColors_.begin() + row);

    if (row < originalMembraneTypeNames_.size())
    {
        removedMembraneTypes_.insert(originalMembraneTypeNames_[row]);
        originalMembraneTypeNames_.erase(originalMembraneTypeNames_.begin() + row);
    }
}

void MembraneTypesTableModel::clearRows()
{
    Base::clearRows();
    membraneColors_.clear();

    for (const auto& membraneType : simulationConfigUpdater_->getSimulationConfig().membraneTypes)
        removedMembraneTypes_.insert(membraneType.name);

    beginInsertRows(QModelIndex(), 0, 0);
    rows_.push_back(simulationConfigUpdater_->getSimulationConfig().cellMembraneType);
    membraneColors_.push_back(sf::Color::Yellow);
    endInsertRows();
}

void MembraneTypesTableModel::addRow()
{
    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));

    rows_.push_back(cell::config::MembraneType{
        .name = "Type" + std::to_string(rows_.size()), .radius = 1, .permeabilityMap = {}, .discCount = 0});
    membraneColors_.push_back(
        getSupportedDiscColors()[static_cast<qsizetype>(membraneColors_.size()) % getSupportedDiscColors().size()]);

    endInsertRows();
}

void MembraneTypesTableModel::loadFromConfig()
{
    removedMembraneTypes_.clear();
    membraneColors_.clear();
    originalMembraneTypeNames_.clear();

    const auto& config = simulationConfigUpdater_->getSimulationConfig();

    beginResetModel();

    rows_ = {config.cellMembraneType};
    rows_.insert(rows_.end(), config.membraneTypes.begin(), config.membraneTypes.end());

    membraneColors_.push_back(sf::Color::Yellow);
    originalMembraneTypeNames_.push_back(config.cellMembraneType.name);
    for (std::size_t i = 1; i < rows_.size(); ++i)
    {
        membraneColors_.push_back(simulationConfigUpdater_->getMembraneTypeColorMap().at(rows_[i].name));
        originalMembraneTypeNames_.push_back(rows_[i].name);
    }

    endResetModel();
}

void MembraneTypesTableModel::saveToConfig()
{
    // Set cell membrane separately
    auto config = simulationConfigUpdater_->getSimulationConfig();
    config.cellMembraneType = rows_[0];
    simulationConfigUpdater_->setSimulationConfig(config);

    std::map<std::string, sf::Color> membraneTypeColorMap;
    membraneTypeColorMap[config.cellMembraneType.name] = sf::Color::Yellow;
    for (std::size_t i = 1; i < membraneColors_.size(); ++i)
        membraneTypeColorMap[rows_[i].name] = membraneColors_[i];

    // Remove cell membrane from list
    auto rows = rows_;
    rows.erase(rows.begin());

    simulationConfigUpdater_->setTypes(rows, removedMembraneTypes_, membraneTypeColorMap);
}

QVariant MembraneTypesTableModel::getField(const cell::config::MembraneType& row, const QModelIndex& index) const
{
    switch (index.column())
    {
    case 0: return QString::fromStdString(row.name);
    case 1: return row.radius;
    case 2: return getColorNameMapping()[membraneColors_[index.row()]];
    case 3:
    case 4: return "Edit";
    case 5: return row.discCount;
    case 6: return "Delete";
    default: return {};
    }
}

bool MembraneTypesTableModel::setField(cell::config::MembraneType& row, const QModelIndex& index, const QVariant& value)
{
    switch (index.column())
    {
    case 0: row.name = value.toString().toStdString(); break;
    case 1: row.radius = value.toDouble(); break;
    case 2: membraneColors_[index.row()] = getNameColorMapping()[value.toString()]; break;
    case 5: row.discCount = value.toInt(); break;
    default: return false;
    }

    return true;
}

bool MembraneTypesTableModel::isEditable(const QModelIndex& index) const
{
    return index.column() != 6;
}

bool MembraneTypesTableModel::isEnabled(const QModelIndex& index) const
{
    if (index.row() == 0)
        return index.column() != 0 && index.column() != 2 && index.column() != 3 && index.column() != 6;

    return true;
}