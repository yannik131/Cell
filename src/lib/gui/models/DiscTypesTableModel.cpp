#include "models/DiscTypesTableModel.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "core/ColorMapping.hpp"

#include "DiscTypesTableModel.hpp"
#include <set>

DiscTypesTableModel::DiscTypesTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QAbstractTableModel(parent)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
{
}

int DiscTypesTableModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(rows_.size());
}

int DiscTypesTableModel::columnCount(const QModelIndex&) const
{
    return 5;
}

QVariant DiscTypesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    static const QStringList Headers{"Name", "Radius", "Mass", "Color", "Delete"};

    return Headers[section];
}

QVariant DiscTypesTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= rows_.size() || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& discType = rows_.at(index.row());

    switch (index.column())
    {
    case 0:
        return QString::fromStdString(discType.name);
    case 1:
        return discType.radius;
    case 2:
        return discType.mass;
    case 3:
        return getColorNameMapping()[discTypeColorMap_.at(discType.name)];
    case 4:
        return "Delete";
    default:
        return {};
    }
}

bool DiscTypesTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() >= rows_.size() || role != Qt::EditRole)
        return false;

    auto& discType = rows_[index.row()];

    switch (index.column())
    {
    case 0:
        discType.name = value.toString().toStdString();
        break;
    case 1:
        discType.radius = value.toDouble();
        break;
    case 2:
        discType.mass = value.toDouble();
        break;
    case 3:
        discTypeColorMap_[discType.name] = getNameColorMapping()[value.toString()];
        break;
    default:
        return false;
    }

    if (index.column() < 4)
        emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags DiscTypesTableModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void DiscTypesTableModel::addEmptyRow()
{
    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));

    rows_.push_back(cell::config::DiscType{.name = "Type" + std::to_string(rows_.size()), .radius = 1, .mass = 1});
    discTypeColorMap_[rows_.back().name] = sf::Color::Blue;

    endInsertRows();
}

void DiscTypesTableModel::removeRow(int row)
{
    if (row < 0 || row >= rows_.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    discTypeColorMap_.erase(rows_[row].name);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();

    const auto& originalDiscTypes = abstractSimulationBuilder_->getSimulationConfig().discTypes;
    if (row < originalDiscTypes.size())
        removedDiscTypes_.insert(originalDiscTypes[row].name);
}

void DiscTypesTableModel::clearRows()
{
    if (rows_.empty())
        return;

    beginRemoveRows(QModelIndex(), 0, static_cast<int>(rows_.size()) - 1);
    rows_.clear();
    discTypeColorMap_.clear();
    endRemoveRows();

    for (const auto& discType : abstractSimulationBuilder_->getSimulationConfig().discTypes)
        removedDiscTypes_.insert(discType.name);
}

void DiscTypesTableModel::commitChanges()
{
    auto config = abstractSimulationBuilder_->getSimulationConfig();
    removeRemovedDiscTypes(config);

    updateDiscTypes(config, rows_);

    abstractSimulationBuilder_->setSimulationConfig(config);
    abstractSimulationBuilder_->setDiscTypeColorMap(discTypeColorMap_);

    removedDiscTypes_.clear();
}

void DiscTypesTableModel::discardChanges()
{
    clearRows();
    removedDiscTypes_.clear();

    auto discTypes = abstractSimulationBuilder_->getSimulationConfig().discTypes;
    if (discTypes.empty())
        return;

    beginInsertRows(QModelIndex(), 0, static_cast<int>(discTypes.size()) - 1);

    rows_ = std::move(discTypes);
    discTypeColorMap_ = abstractSimulationBuilder_->getDiscTypeColorMap();

    endInsertRows();
}

void DiscTypesTableModel::removeRemovedDiscTypes(cell::SimulationConfig& config)
{
    config.discTypes.erase(std::remove_if(config.discTypes.begin(), config.discTypes.end(),
                                          [&](const cell::config::DiscType& discType)
                                          { return removedDiscTypes_.contains(discType.name); }),
                           config.discTypes.end());

    config.reactions.erase(std::remove_if(config.reactions.begin(), config.reactions.end(),
                                          [&](const cell::config::Reaction& reaction)
                                          {
                                              return removedDiscTypes_.contains(reaction.educt1) ||
                                                     removedDiscTypes_.contains(reaction.educt2) ||
                                                     removedDiscTypes_.contains(reaction.product1) ||
                                                     removedDiscTypes_.contains(reaction.product2);
                                          }),
                           config.reactions.end());

    for (auto iter = config.setup.distribution.begin(); iter != config.setup.distribution.end();)
    {
        if (removedDiscTypes_.contains(iter->first))
            iter = config.setup.distribution.erase(iter);
        else
            ++iter;
    }

    config.setup.discs.erase(std::remove_if(config.setup.discs.begin(), config.setup.discs.end(),
                                            [&](const cell::config::Disc& disc)
                                            { return removedDiscTypes_.contains(disc.discTypeName); }),
                             config.setup.discs.end());
}

void DiscTypesTableModel::updateDiscTypes(cell::SimulationConfig& config,
                                          const std::vector<cell::config::DiscType>& updatedDiscTypes)
{
    if (config.discTypes.size() > updatedDiscTypes.size())
        throw std::logic_error(
            "The original configuration can't contain more disc types than the new one after deleting deleted ones.");

    // We'll map "" to "" to accomodate empty strings like in reactions
    std::unordered_map<std::string, std::string> changeMap({{"", ""}});
    for (std::size_t i = 0; i < config.discTypes.size(); ++i)
        changeMap[config.discTypes[i].name] = updatedDiscTypes[i].name;

    for (auto& reaction : config.reactions)
    {
        reaction.educt1 = changeMap[reaction.educt1];
        reaction.educt2 = changeMap[reaction.educt2];
        reaction.product1 = changeMap[reaction.product1];
        reaction.product2 = changeMap[reaction.product2];
    }

    std::map<std::string, double> newDistribution;
    for (const auto& [discType, frequency] : config.setup.distribution)
        newDistribution[changeMap[discType]] = frequency;
    config.setup.distribution = std::move(newDistribution);

    for (auto& disc : config.setup.discs)
        disc.discTypeName = changeMap[disc.discTypeName];

    config.discTypes = updatedDiscTypes;
}
