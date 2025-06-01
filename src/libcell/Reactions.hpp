#ifndef REACTIONS_HPP
#define REACTIONS_HPP

/**
 * @brief Internal reaction functions
 */

#include <vector>

class Disc;

bool combinationReaction(Disc* d1, Disc* d2);

bool exchangeReaction(Disc* d1, Disc* d2);

bool decompositionReaction(Disc* d1, std::vector<Disc>& newDiscs);

bool transformationReaction(Disc* d1);

/**
 * @brief Lets the discs in the passed vector decompose according to the schemes A -> B + C defined in the settings with
 * the corresponding probabilities (per second)
 */
std::vector<Disc> unimolecularReactions(std::vector<Disc>& discs);

#endif /* REACTIONS_HPP */
