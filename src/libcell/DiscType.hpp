#ifndef DISCTYPE_HPP
#define DISCTYPE_HPP

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
    struct IdHasher
    {
        int operator()(const DiscType& discType) const;
    };

    struct PairHasher
    {
        int operator()(const std::pair<DiscType, DiscType>& pair) const;
    };

    struct IdComparator
    {
        bool operator()(const DiscType& a, const DiscType& b) const
        {
            return a.getId() == b.getId();
        }
    };

    struct PairComparator
    {
        bool operator()(const std::pair<DiscType, DiscType>& a, const std::pair<DiscType, DiscType>& b) const
        {
            return a.first.getId() == b.first.getId() && a.second.getId() == b.second.getId();
        }
    };

public:
    /**
     * @brief Type to be used for reaction and disc type distribution tables
     */
    template <typename T> using map = std::unordered_map<DiscType, T, IdHasher, IdComparator>;

    /**
     * @brief Type for the bimolecular reaction tables
     */
    template <typename T>
    using pair_map = std::unordered_map<std::pair<DiscType, DiscType>, T, PairHasher, PairComparator>;

public:
    /**
     * @attention Just here for nlohmann::json, shouldn't be used
     */
    DiscType() = default;

    /**
     * @brief Creates a new disc type with unique ID
     */
    DiscType(const std::string& name, const sf::Color& color, float radius, float mass);

    /**
     * @brief Copy-ctor, copies the ID
     */
    DiscType(const DiscType& other);

    /**
     * @brief Assignment operator, copies the ID
     */
    DiscType& operator=(const DiscType& other);

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
    float getRadius() const;

    /**
     * @brief Sets the radius of this DiscType in px (must be > 0)
     */
    void setRadius(float radius);

    /**
     * @returns The mass of this DiscType (arbitrary unit right now)
     */
    float getMass() const;

    /**
     * @brief Sets the mass of this DiscType (must be > 0, arbitrary unit)
     */
    void setMass(float mass);

    /**
     * @returns The unique ID of this DiscType (unless it was copied)
     * @note This was a mistake. Since there are only a handful of different DiscType instances in the lifetime of the
     * simulation, this class should have been uncopyable with the central instances in the settings and every other
     * class should have had pointers to those instances. This would have made IDs unnecessary since updating a disc
     * type somewhere would immediately update it everywhere. Also, with this current ID approach, serialization with
     * nlohmann::json requires a default constructor that I don't want to have, since the ID cannot be computed from any
     * other property and has to be saved separately.
     */
    int getId() const;

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
     * @todo https://github.com/yannik131/Cell/issues/25
     */
    float radius_ = 0;

    /**
     * @brief Mass in arbitrary unit
     */
    float mass_ = 0;

    /**
     * @brief A unique identifier of this object that is copied by the copy-ctor and operator=
     * @note Necessary so I can check if a DiscType changed by the user was part of a reaction so that the reaction can
     * be updated accordingly
     */
    int id_ = 0;

    /**
     * @brief Instance counter used for the ID
     */
    static int instanceCount;

public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DiscType, name_, color_, radius_, mass_, id_)
};

/**
 * @brief Creates an ordered pair sorted by ID
 */
std::pair<DiscType, DiscType> makeOrderedPair(const DiscType& d1, const DiscType& d2);

} // namespace cell

namespace nlohmann
{

template <> struct adl_serializer<std::optional<cell::DiscType>>
{
    static void to_json(json& j, const std::optional<cell::DiscType>& opt)
    {
        if (opt.has_value())
            j = *opt;
        else
            j = nullptr;
    }

    static void from_json(const json& j, std::optional<cell::DiscType>& opt)
    {
        if (j.is_null())
            opt = std::nullopt;
        else
            opt = std::make_optional(j.get<cell::DiscType>());
    }
};

} // namespace nlohmann

#endif /* DISCTYPE_HPP */
