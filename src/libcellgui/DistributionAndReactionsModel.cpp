#include "DistributionAndReactionsModel.hpp"

void DistributionAndReactionsModel::updateDiscType(const DiscTypeInput& discTypeInput)
{
}

void DistributionAndReactionsModel::removeDiscType(const QString& name)
{
}

std::map<DiscType, int> DistributionAndReactionsModel::getDiscTypeDistribution() const
{
    return std::map<DiscType, int>();
}

void DistributionAndReactionsModel::updateReaction(const ReactionInput& reactionInput)
{
}

void DistributionAndReactionsModel::removeReaction(int id)
{
}

std::map<std::pair<DiscType, DiscType>, std::vector<std::pair<DiscType, float>>>
DistributionAndReactionsModel::getCombinationReactionsTable() const
{
    return std::map<std::pair<DiscType, DiscType>, std::vector<std::pair<DiscType, float>>>();
}

std::map<DiscType, std::vector<std::pair<std::pair<DiscType, DiscType>, float>>>
DistributionAndReactionsModel::getDecompositionReactionsTable() const
{
    return std::map<DiscType, std::vector<std::pair<std::pair<DiscType, DiscType>, float>>>();
}
