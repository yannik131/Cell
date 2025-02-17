#ifndef DISC_HPP
#define DISC_HPP

#include "DiscType.hpp"

#include <SFML/System/Vector2.hpp>

struct Disc {
    Disc(const DiscType& discType);

    sf::Vector2f velocity_;
    sf::Vector2f position_;

    DiscType type_;
};

#endif /* DISC_HPP */
