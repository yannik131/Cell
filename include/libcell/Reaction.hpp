#ifndef REACTION_HPP
#define REACTION_HPP

#include "DiscType.hpp"

#include <vector>

struct Reaction
{
    DiscType educt1_;
    DiscType educt2_;
    DiscType product1_;
    DiscType product2_;
    float probability_;
};

bool operator==(const Reaction& reaction1, const Reaction& reaction2);
std::string toString(const Reaction& reaction);
bool contains(const Reaction& reaction, const DiscType& discType);

void addReactionToVector(std::vector<Reaction>& reactions, Reaction reaction);
void removeReactionFromVector(std::vector<Reaction>& reactions, Reaction reaction);
void removeReactionsFromVector(std::vector<Reaction>& reactions, const DiscType& discType);

#endif /* REACTION_HPP */
