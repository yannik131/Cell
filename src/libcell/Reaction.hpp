#ifndef REACTION_HPP
#define REACTION_HPP

#include "DiscType.hpp"

#include <optional>
#include <vector>

namespace cell
{

/**
 * @brief Contains a uni- or bimolecular reaction.
 *
 * Given `DiscType`s A, B, C and D, and reaction probability 0 <= p <=
 * 1, supported reaction types are:
 * ```cpp
 * Reaction(A, nullopt, B, nullopt, p); // Transformation reaction: A -> B
 * Reaction(A, nullopt, B, C, p); // Decomposition reaction: A -> B + C
 * Reaction(A, B, C, nullopt, p); // Combination reaction: A + B -> C
 * Reaction(A, B, C, D, p); // Exchange reaction: A + B -> C + D
 * ```
 *
 * For transformation and decomposition reactions, p is interpreted in terms of probability per second for each disc in
 * the simulation. For combination and exchange reactions, p is the reaction probability for a single collision
 */
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
    /**
     * @brief Creates a new reaction, inferring the type from the provided arguments. Throws if the given probability is
     * not in the interval [0, 1]
     */
    Reaction(const DiscType& educt1, const std::optional<DiscType>& educt2, const DiscType& product1,
             const std::optional<DiscType>& product2, float probability);

    // Boilerplate getters and setters with no additional documentation

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

    /**
     * @brief Validates that
     *
     * - educts and products of transformation reactions A -> B are not identical (A must be unequal B)
     *
     * - educt and product masses are the same
     */
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

/**
 * @brief String representation in the form of A + B -> C + D
 */
std::string toString(const Reaction& reaction);

/**
 * @returns `true` if the given disctype is part of the educts or products of the reaction
 */
bool contains(const Reaction& reaction, const DiscType& discType);

} // namespace cell

#endif /* REACTION_HPP */
