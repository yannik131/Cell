#include "DiscTypeRegistry.hpp"
#include "DiscType.hpp"
#include "ExceptionWithLocation.hpp"

#include <limits>

namespace cell
{

void cell::DiscTypeRegistry::setDiscTypes(std::vector<DiscType>&& discTypes)
{
    if (discTypes.empty())
    {
        discTypes_.clear();
        return;
    }

    const auto& MaxCount = std::numeric_limits<DiscTypeID>::max();
    if (discTypes.size() > MaxCount)
        throw ExceptionWithLocation("Too many disc types: Registry only supports " + std::to_string(MaxCount) +
                                    ", but " + std::to_string(discTypes.size()) + " were given");

    buildNameIDMap(discTypes);

    discTypes_ = std::move(discTypes);
    maxRadius_ = std::max_element(discTypes_.begin(), discTypes_.end(),
                                  [](const DiscType& a, const DiscType& b) { return a.getRadius() < b.getRadius(); })
                     ->getRadius();
}

DiscTypeID DiscTypeRegistry::getIDFor(const std::string& discTypeName) const
{
    if (!nameIDMap_.contains(discTypeName))
        throw ExceptionWithLocation("Couldn't find disc type with name \"" + discTypeName + "\" in registry");

    return nameIDMap_.at(discTypeName);
}

const DiscType& DiscTypeRegistry::getByID(DiscTypeID ID) const
{
#ifdef DEBUG
    if (static_cast<std::size_t>(ID) >= discTypes_.size())
        throw ExceptionWithLocation("Index out of range: " + std::to_string(ID) + ", registry size is " +
                                    std::to_string(discTypes_.size()));
#endif

    return discTypes_[ID];
}

double DiscTypeRegistry::getMaxRadius() const
{
    return maxRadius_;
}

DiscTypeResolver DiscTypeRegistry::getDiscTypeResolver() const
{
    return [&](DiscTypeID discTypeID) -> const DiscType& { return getByID(discTypeID); };
}

void DiscTypeRegistry::buildNameIDMap(const std::vector<DiscType>& discTypes)
{
    std::unordered_map<std::string, DiscTypeID> nameIDMap;

    for (DiscTypeID i = 0; i < static_cast<DiscTypeID>(discTypes.size()); ++i)
    {
        if (nameIDMap.contains(discTypes[i].getName()))
            throw ExceptionWithLocation("Can't build name map: Duplicate disc type name \"" + discTypes[i].getName() +
                                        "\"");

        nameIDMap[discTypes[i].getName()] = i;
    }

    nameIDMap_ = std::move(nameIDMap);
}

} // namespace cell