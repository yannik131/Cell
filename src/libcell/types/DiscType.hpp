#ifndef DISCTYPE_HPP
#define DISCTYPE_HPP

#include "SFMLJsonSerializers.hpp"
#include "TypeBase.hpp"

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
class DiscType : public TypeBase
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
    DiscType(const std::string& name, const sf::Color& color, double radius, double mass);

    /**
     * @brief Assignment operator, copies the ID
     */
    DiscType& operator=(const DiscType& other);

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
     * @todo https://github.com/yannik131/Cell/issues/25
     */
    double radius_ = 0;

    /**
     * @brief Mass in arbitrary unit
     */
    double mass_ = 0;

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
