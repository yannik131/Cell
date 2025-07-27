#ifndef REACTIONS_HPP
#define REACTIONS_HPP

/**
 * @brief Internal reaction functions. See doc/physics/ for mathematical details. All these functions
 *
 * - Search the respective lookup map in the `ReactionTable` for any reactions that match the given disc types
 *
 * - Might let a single reaction occur based on their probabilities
 *
 * - Return `true` if a reaction occured and `false` otherwise
 */

#include <vector>

namespace cell
{

class Disc;

namespace reactions
{

/**
 * @brief Transformation reaction A -> B. Changes the type of the disc to a new one if a reaction occurs.
 * @param d1 The disc to transform
 */
bool transformationReaction(Disc* d1);

/**
 * @brief Decomposition reaction A -> B + C. Destroys the old disc and creates 2 new ones if a reaction occurs.
 * @param d1 The disc to decompose
 * @param newDiscs vector to store the 2 new discs in
 */
bool decompositionReaction(Disc* d1, std::vector<Disc>& newDiscs);

/**
 * @brief Combination reaction A + B -> C. Destroys one of the 2 educt discs and changes the other if a reaction occurs.
 * @param d1 Colliding disc 1
 * @param d2 Colliding disc 2
 */
bool combinationReaction(Disc* d1, Disc* d2);

/**
 * @brief Exchange reaction A + B -> C + D. Just changes the disc types of the reacting discs.
 * @param d1 Colliding disc 1
 * @param d2 Colliding disc 2
 */
bool exchangeReaction(Disc* d1, Disc* d2);

/**
 * @brief Lets decomposition and transformation reactions occur for the passed discs. Only 1 of the 2 reaction types
 * will occur for each disc.
 * @param discs Discs to transform/decompose
 */
std::vector<Disc> unimolecularReactions(std::vector<Disc>& discs);

} // namespace reactions

} // namespace cell

#endif /* REACTIONS_HPP */
