#ifndef DISCTYPE_HPP
#define DISCTYPE_HPP

#include "Hashing.hpp"
#include "SFMLJsonSerializers.hpp"

#include <SFML/Graphics/Color.hpp>
#include <nlohmann/json.hpp>

#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace cell
{

/**
 * @brief Contains the physical and visual properties of a disc. Disc types used in the simulation need to be distinct
 * by name
 */
class DiscType
{
public:
    /**
     * @brief Type to be used for reaction and disc type distribution tables
     */
    template <typename T> using map = std::unordered_map<const DiscType*, T>;

    /**
     * @brief Type for the bimolecular reaction tables
     */
    template <typename T>
    using pair_map = std::unordered_map<std::pair<const DiscType*, const DiscType*>, T, PairHasher>;

public:
    /**
     * @brief Creates a new disc type
     */
    DiscType(const std::string& name, const sf::Color& color, double radius, double mass);

    /**
     * @brief Disc types should be shared across all discs and be unique
     */
    DiscType(const DiscType&) = delete;
    DiscType& operator=(const DiscType&) = delete;
    DiscType(DiscType&&) = default;
    DiscType& operator=(DiscType&&) = default;

    /**
     * @returns The name of this DiscType
     */
    const std::string& getName() const;

    /**
     * @brief Sets the name for this DiscType (can't be empty)
     */
    void setName(const std::string& name);

    /**
     * @returns The color of this DiscType
     */
    const sf::Color& getColor() const;

    /**
     * @brief Sets the color of this DiscType. Currently, only the predefined sf::Color static members are allowed
     * (except sf::Black because of the black simulation background)
     */
    void setColor(const sf::Color& color);

    /**
     * @returns The radius of this DiscType in px
     */
    double getRadius() const;

    /**
     * @brief Sets the radius of this DiscType in px (must be > 0)
     */
    void setRadius(double radius);

    /**
     * @returns The mass of this DiscType (arbitrary unit right now)
     */
    double getMass() const;

    /**
     * @brief Sets the mass of this DiscType (must be > 0, arbitrary unit)
     */
    void setMass(double mass);

    /**
     * @brief Comparison by all members
     */
    bool operator==(const DiscType& other) const;

private:
    /**
     * @brief The name is used to uniquely identify disc types within the disc type distribution
     */
    std::string name_;

    /**
     * @brief Visual color of the DiscType
     */
    sf::Color color_;

    /**
     * @brief Radius in px
     */
    double radius_ = 0;

    /**
     * @brief Mass in arbitrary unit
     */
    double mass_ = 0;
};

} // namespace cell

/* namespace nlohmann
{

template <> struct adl_serializer<cell::DiscType>
{
    static void to_json(json& j, const cell::DiscType& d)
    {
        j = json{{"name", d.getName()}, {"color", d.getColor()}, {"radius", d.getRadius()}, {"mass", d.getMass()}};
    }

    static cell::DiscType from_json(const json& j, const cell::DiscType& d)
    {
        return cell::DiscType{j.at("name").get<std::string>(), j.at("color").get<sf::Color>(),
                              j.at("radius").get<double>(), j.at("mass").get<double>()};
    }
};

} // namespace nlohmann */

#endif /* DISCTYPE_HPP */
