#ifndef REACTIONS_HPP
#define REACTIONS_HPP

/**
 * @brief Internal reaction functions
 */

#include <vector>

class Disc;

bool combinationReaction(Disc* d1, Disc* d2);

bool exchangeReaction(Disc* d1, Disc* d2);

void decompositionReaction(Disc* d1, std::vector<Disc>& newDiscs);

#endif /* REACTIONS_HPP */
