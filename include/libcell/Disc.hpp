#ifndef DISC_HPP
#define DISC_HPP

#include "DiscType.hpp"

#include <SFML/System/Vector2.hpp>

struct Disc
{
    Disc(const DiscType& discType);

    sf::Vector2f velocity_;
    sf::Vector2f position_;

    /**
     * @brief Reactions of type A + B -> C require B to be removed (A can be changed to C). This flag
     * is set during the reaction processing and the world removed flagged discs after the update step
     */
    bool destroyed_ = false;

    /**
     * @brief Set when a disc changes types, for example in A + B -> C, we keep A and change it to C and destroy B. The
     * UI needs to be informed of this to update the discs accordingly, which this flag is for
     */
    bool changed_ = false;
    DiscType type_;

    int getId() const;

    float getAbsoluteMomentum() const;
    float getKineticEnergy() const;

private:
    static int instanceCount;
    int id_;
};

#endif /* DISC_HPP */
