#ifndef ABSTRACTREACTIONTABLE_HPP
#define ABSTRACTREACTIONTABLE_HPP

#include "Types.hpp"

namespace cell
{

class AbstractReactionTable
{
public:
    virtual const DiscTypeMap<std::vector<Reaction>>& getTransformations() const = 0;
    virtual const DiscTypeMap<std::vector<Reaction>>& getDecompositions() const = 0;
    virtual const DiscTypePairMap<std::vector<Reaction>>& getCombinations() const = 0;
    virtual const DiscTypePairMap<std::vector<Reaction>>& getExchanges() const = 0;

    virtual ~AbstractReactionTable() = default;
};

} // namespace cell

#endif /* ABSTRACTREACTIONTABLE_HPP */
