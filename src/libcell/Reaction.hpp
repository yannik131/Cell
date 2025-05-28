#ifndef REACTION_HPP
#define REACTION_HPP

#include "DiscType.hpp"

#include <optional>
#include <vector>

class Reaction
{
public:
    enum Type
    {
        Transformation = 1 << 0,
        Decomposition = 1 << 1,
        Combination = 1 << 2,
        Exchange = 1 << 3
    };

public:
    Reaction(const DiscType& educt1, const std::optional<DiscType>& educt2, const DiscType& product1,
             const std::optional<DiscType>& product2, float probability);

    const DiscType& getEduct1() const;
    void setEduct1(const DiscType& educt1);

    const DiscType& getEduct2() const;
    bool hasEduct2() const;
    void setEduct2(const DiscType& educt2);

    const DiscType& getProduct1() const;
    void setProduct1(const DiscType& product1);

    const DiscType& getProduct2() const;
    bool hasProduct2() const;
    void setProduct2(const DiscType& product2);

    float getProbability() const;
    void setProbability(float probability);

    const Type& getType() const;

    void validate() const;

private:
    DiscType educt1_;
    std::optional<DiscType> educt2_;
    DiscType product1_;
    std::optional<DiscType> product2_;
    float probability_ = 0;
    Type type_;
};

struct ReactionHash
{
    size_t operator()(const Reaction& reaction) const;
};

/**
 * @brief Checks if all products and educts have identical disc type names
 */
bool operator==(const Reaction& reaction1, const Reaction& reaction2);
std::string toString(const Reaction& reaction);
bool contains(const Reaction& reaction, const DiscType& discType);
void addReactionToVector(std::vector<Reaction>& reactions, Reaction reaction);

#endif /* REACTION_HPP */
