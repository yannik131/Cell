#ifndef REACTIONENGINE_HPP
#define REACTIONENGINE_HPP

#include "DiscTypeRegistry.hpp"

#include <functional>

namespace cell
{

class DiscType;
class Disc;

class ReactionEngine
{
public:
    using SingleLookupMap = DiscTypeMap<std::vector<Reaction>>;
    using PairLookupMap = DiscTypePairMap<std::vector<Reaction>>;

public:
    ReactionEngine(DiscTypeResolver discTypeResolver, const SingleLookupMap& decompositions,
                   const SingleLookupMap& transformations, const PairLookupMap& combinations,
                   const PairLookupMap& exchanges);

    /**
     * @brief Transformation reaction A -> B. Changes the type of the disc to a new one if a reaction occurs.
     * @param d1 The disc to transform
     */
    bool transformationReaction(Disc* d1) const;

    /**
     * @brief Decomposition reaction A -> B + C. Destroys the old disc and creates 2 new ones if a reaction occurs.
     * @param d1 The disc to decompose
     * @param newDiscs vector to store the 2 new discs in
     */
    bool decompositionReaction(Disc* d1, std::vector<Disc>& newDiscs) const;

    /**
     * @brief Combination reaction A + B -> C. Destroys one of the 2 educt discs and changes the other if a reaction
     * occurs.
     * @param d1 Colliding disc 1
     * @param d2 Colliding disc 2
     */
    bool combinationReaction(Disc* d1, Disc* d2) const;

    /**
     * @brief Exchange reaction A + B -> C + D. Just changes the disc types of the reacting discs.
     * @param d1 Colliding disc 1
     * @param d2 Colliding disc 2
     */
    bool exchangeReaction(Disc* d1, Disc* d2) const;

    /**
     * @brief Lets decomposition and transformation reactions occur for the passed discs. Only 1 of the 2 reaction types
     * will occur for each disc.
     * @param discs Discs to transform/decompose
     */
    std::vector<Disc> unimolecularReactions(std::vector<Disc>& discs) const;

private:
    DiscTypeResolver discTypeResolver_;
    const SingleLookupMap* transformations_;
    const SingleLookupMap* decompositions_;
    const PairLookupMap* combinations_;
    const PairLookupMap* exchanges_;
};

} // namespace cell

#endif /* REACTIONENGINE_HPP */
