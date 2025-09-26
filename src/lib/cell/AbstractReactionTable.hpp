#ifndef DA4C3E36_2329_41AE_8143_E132E827F27B_HPP
#define DA4C3E36_2329_41AE_8143_E132E827F27B_HPP

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

#endif /* DA4C3E36_2329_41AE_8143_E132E827F27B_HPP */
