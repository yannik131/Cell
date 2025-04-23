#ifndef DISCTYPE_HPP
#define DISCTYPE_HPP

#include <SFML/Graphics/Color.hpp>

#include <string>
#include <utility>

/**
 * @brief Struct containing the defining properties of a disc
 */
class DiscType
{
public:
    DiscType(const std::string& name, const sf::Color& color, float radius, float mass);
    DiscType(const DiscType& other);
    DiscType& operator=(const DiscType& other);

    const std::string& getName() const;
    void setName(const std::string& name);

    const sf::Color& getColor() const;
    void setColor(const sf::Color& color);

    float getRadius() const;
    void setRadius(float radius);

    float getMass() const;
    void setMass(float mass);

    int getId() const;

private:
    std::string name_;
    sf::Color color_;
    float radius_ = 0;
    float mass_ = 0;

    /**
     * @brief A unique identifier of this object that is copied by the copy-ctor and operator=
     * @note Necessary so I can check if a DiscType changed by the user was part of a reaction so that the reaction can
     * be updated accordingly
     */
    int id_;

    static int instanceCount;
};

/**
 * @brief 2 disc types are equal if there names are equal
 */
bool operator==(const DiscType& a, const DiscType& b);

/**
 * @brief This has no semantic meaning, it's just an implementation detail for using std::map and std::set etc.
 */
bool operator<(const DiscType& a, const DiscType& b);

/**
 * @brief Sorts the types by name in the pair
 */
std::pair<DiscType, DiscType> makeOrderedPair(const DiscType& d1, const DiscType& d2);

#endif /* DISCTYPE_HPP */
