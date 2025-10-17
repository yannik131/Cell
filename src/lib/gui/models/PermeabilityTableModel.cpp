#include "models/PermeabilityTableModel.hpp"
#include "cell/DiscType.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "core/ColorMapping.hpp"
#include "core/PermeabilityMapping.hpp"

#include "PermeabilityTableModel.hpp"
#include <algorithm>

PermeabilityTableModel::PermeabilityTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QAbstractTableModel(parent)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
{
}

int PermeabilityTableModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(rows_.size());
}

int PermeabilityTableModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

QVariant PermeabilityTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    static const QStringList Headers{"Disc type", "Radius", "Mass", "Color", "Permeability"};

    return Headers[section];
}

QVariant PermeabilityTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= static_cast<int>(rows_.size()) || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& discTypes = abstractSimulationBuilder_->getDiscTypeRegistry().getValues();
    const auto& permeability = rows_.at(index.row());

    switch (index.column())
    {
    case 0:
        return QString::fromStdString(discTypes[index.row()].getName());
    case 1:
        return discTypes[index.row()].getRadius();
    case 2:
        return discTypes[index.row()].getMass();
    case 3:
    {
        const auto& color = abstractSimulationBuilder_->getDiscTypeColorMap().at(discTypes[index.row()].getName());
        return getColorNameMapping()[color];
    }
    case 4:
        return getPermeabilityNameMapping()[permeability];
    default:
        return {};
    }
}

bool PermeabilityTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() >= rows_.size() || role != Qt::EditRole)
        return false;

    auto& permeability = rows_[index.row()];

    switch (index.column())
    {
    case 4:
        permeability = getNamePermeabilityMapping()[value.toString()];
        break;
    default:
        return false;
    }

    if (index.column() == 4)
        emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags PermeabilityTableModel::flags(const QModelIndex& index) const
{
    if (index.column() == 4)
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return Qt::ItemIsSelectable;
}

void PermeabilityTableModel::loadMembraneType(const std::string& membraneTypeName)
{
    const auto& membraneTypes = abstractSimulationBuilder_->getSimulationConfig().membraneTypes;
    const auto membraneTypeIter = std::find_if(membraneTypes.begin(), membraneTypes.end(),
                                               [&](const auto& type) { return type.name == membraneTypeName; });
    const auto& discTypes = abstractSimulationBuilder_->getSimulationConfig().discTypes;

    beginResetModel();
    rows_.clear();

    if (membraneTypeIter == membraneTypes.end())
    {
        for (const auto& discType : discTypes)
            rows_.push_back(cell::MembraneType::Permeability::None);
    }
    else
    {
        const auto& permeabilityMap = membraneTypeIter->permeabilityMap;

        for (const auto& discType : discTypes)
        {
            const auto iter = permeabilityMap.find(discType.name);
            if (iter == permeabilityMap.end())
                rows_.push_back(cell::MembraneType::Permeability::None);
            else
                rows_.push_back(iter->second);
        }
    }

    endResetModel();
}
