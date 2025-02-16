#ifndef DISCTYPE_HPP
#define DISCTYPE_HPP

#include <SFML/Graphics/Color.hpp>

#include <string>

struct DiscType;

bool operator==(const DiscType& a, const DiscType& b);
bool operator<(const DiscType& a, const DiscType& b);

struct DiscType
{
    DiscType() = default;
    DiscType(const std::string& name, const sf::Color& color, float radius, float mass);

    std::string name_;
    sf::Color color_;
    float radius_;
    float mass_;
};

#endif /* DISCTYPE_HPP */
