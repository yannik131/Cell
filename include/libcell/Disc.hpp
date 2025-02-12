#ifndef DISC_HPP
#define DISC_HPP

#include <SFML/System/Vector2.hpp>

struct Disc {
    Disc(float radius);

    sf::Vector2f velocity_;
    sf::Vector2f position_;
    float radius_;
    float mass_;
};

#endif /* DISC_HPP */
