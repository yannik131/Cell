#ifndef ABSTRACTREACTIONTABLE_HPP
#define ABSTRACTREACTIONTABLE_HPP

#include "Types.hpp"

namespace cell
{

class AbstractReactionTable
{
public:
    virtual const DiscTypeMap<std::vector<Reaction>>& getTransformationReactionLookupMap() const = 0;
    virtual const DiscTypeMap<std::vector<Reaction>>& getDecompositionReactionLookupMap() const = 0;
    virtual const DiscTypePairMap<std::vector<Reaction>>& getCombinationReactionLookupMap() const = 0;
    virtual const DiscTypePairMap<std::vector<Reaction>>& getExchangeReactionLookupMap() const = 0;

    virtual ~AbstractReactionTable() = default;
};

} // namespace cell

#endif /* ABSTRACTREACTIONTABLE_HPP */
