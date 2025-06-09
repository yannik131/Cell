#ifndef DISCTYPE_HPP
#define DISCTYPE_HPP

#include <SFML/Graphics/Color.hpp>

#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

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

public:
    /**
     * @brief Type to be used for reaction and disc type distribution tables
     */
    template <typename T> using map = std::unordered_map<DiscType, T, IdHasher>;

    /**
     * @brief Type for the bimolecular reaction tables
     */
    template <typename T> using pair_map = std::unordered_map<std::pair<DiscType, DiscType>, T, PairHasher>;

public:
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
     */
    int getId() const;

    /**
     * @brief Comparison by ID
     */
    bool operator==(const DiscType& other) const;

    /**
     * @brief Comparison by all members
     */
    bool equalsTo(const DiscType& other) const;

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
    int id_;

    /**
     * @brief Instance counter used for the ID
     */
    static int instanceCount;
};

/**
 * @brief Creates an ordered pair sorted by ID
 */
std::pair<DiscType, DiscType> makeOrderedPair(const DiscType& d1, const DiscType& d2);

#endif /* DISCTYPE_HPP */
