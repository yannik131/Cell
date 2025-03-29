#ifndef DISCTYPE_HPP
#define DISCTYPE_HPP

#include <SFML/Graphics/Color.hpp>

#include <string>
#include <utility>

/**
 * @brief Struct containing the defining properties of a disc
 */
struct DiscType
{
    std::string name_;
    sf::Color color_;
    float radius_;
    float mass_;
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

/**
 * @brief Checks if the values make sense (used for checking user defined disc types)
 */
bool isValid(const DiscType& discType);

#endif /* DISCTYPE_HPP */
