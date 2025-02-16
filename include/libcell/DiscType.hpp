#ifndef DISCTYPE_HPP
#define DISCTYPE_HPP

#include <SFML/Graphics/Color.hpp>

#include <string>
#include <vector>

struct DiscType;

bool operator==(const DiscType& a, const DiscType& b);
bool operator<(const DiscType& a, const DiscType& b);

const std::vector<sf::Color> SupportedDicsColors{sf::Color::White, sf::Color::Red,    sf::Color::Green,
                                                 sf::Color::Blue,  sf::Color::Yellow, sf::Color::Magenta,
                                                 sf::Color::Cyan};

struct DiscType
{
    std::string name_;
    sf::Color color_;
    float radius_;
    float mass_;
};

#endif /* DISCTYPE_HPP */
