#ifndef REACTION_HPP
#define REACTION_HPP

#include "DiscType.hpp"
#include "DiscTypeRegistry.hpp"
#include "SFMLJsonSerializers.hpp"

#include <nlohmann/json.hpp>

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
        Exchange = 1 << 3,
        None = 0
    };

public:
    /**
     * @brief Default ctor for json, don't use
     */
    Reaction() = default;

    /**
     * @brief Creates a new reaction, inferring the type from the provided arguments. Throws if the given probability is
     * not in the interval [0, 1]
     */
    Reaction(DiscTypeID educt1, const std::optional<DiscTypeID>& educt2, DiscTypeID product1,
             const std::optional<DiscTypeID>& product2, double probability);

    // Boilerplate getters and setters with no additional documentation

    // TODO remove these and other unneeded functions (maybe?)
    DiscTypeID getEduct1() const;
    void setEduct1(DiscTypeID educt1);

    DiscTypeID getEduct2() const;
    bool hasEduct2() const;
    void setEduct2(DiscTypeID educt2);

    DiscTypeID getProduct1() const;
    void setProduct1(DiscTypeID product1);

    DiscTypeID getProduct2() const;
    bool hasProduct2() const;
    void setProduct2(DiscTypeID product2);

    double getProbability() const;
    void setProbability(double probability);

    const Type& getType() const;

    /**
     * @brief Validates that
     *
     * - educts and products of transformation reactions A -> B are not identical (A must be unequal B)
     *
     * - educt and product masses are the same
     */
    void validate(const DiscTypeResolver& discTypeResolver) const;

private:
    DiscTypeID educt1_;
    std::optional<DiscTypeID> educt2_;
    DiscTypeID product1_;
    std::optional<DiscTypeID> product2_;
    double probability_ = 0;
    Type type_ = Type::None;
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
std::string toString(const Reaction& reaction, const DiscTypeResolver& discTypeResolver);

/**
 * @returns `true` if the given disctype is part of the educts or products of the reaction
 */
bool contains(const Reaction& reaction, DiscTypeID discType);

} // namespace cell

/* namespace nlohmann
{
template <> struct adl_serializer<cell::Reaction>
{
    static void to_json(json& j, const cell::Reaction& reaction)
    {
        j = {};
        j["educt1"] = reaction.getEduct1()->getName();
        j["educt2"] = reaction.hasEduct2() ? reaction.getEduct2()->getName() : nullptr;
        j["product1"] = reaction.getProduct1()->getName();
        j["product2"] = reaction.hasProduct2() ? reaction.getProduct2()->getName() : nullptr;
        j["probability"] = reaction.getProbability();
    }

    static cell::Reaction from_json(const json& j)
    {
        return cell::Reaction();
    }
};
} // namespace nlohmann */

#endif /* REACTION_HPP */
