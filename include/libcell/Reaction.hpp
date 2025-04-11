#ifndef REACTION_HPP
#define REACTION_HPP

#include "DiscType.hpp"

#include <vector>

// TODO turn into a class so that only valid reactions can be created/updated, type can't change and is nested type
// remove validation in global settings later
// use type to set isEditable flags in the reactionsModel
struct Reaction
{
    DiscType educt1_;
    DiscType educt2_;
    DiscType product1_;
    DiscType product2_;
    float probability_ = 0;
};

enum ReactionType
{
    Decomposition,
    Combination,
    Exchange,
    Invalid
};

ReactionType inferReactionType(const Reaction& reaction);

struct ReactionHash
{
    size_t operator()(const Reaction& reaction) const;
};

bool operator==(const Reaction& reaction1, const Reaction& reaction2);
std::string toString(const Reaction& reaction);
bool contains(const Reaction& reaction, const DiscType& discType);

void addReactionToVector(std::vector<Reaction>& reactions, Reaction reaction);
void removeReactionFromVector(std::vector<Reaction>& reactions, Reaction reaction);
void removeReactionsFromVector(std::vector<Reaction>& reactions, const DiscType& discType);

#endif /* REACTION_HPP */
