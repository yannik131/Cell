#ifndef DISC_HPP
#define DISC_HPP

#include "DiscType.hpp"

#include <SFML/System/Vector2.hpp>

class Disc
{
public:
    Disc(const DiscType& discType);

    void setVelocity(const sf::Vector2f& velocity);

    void scaleVelocity(float factor);

    void accelerate(const sf::Vector2f& acceleration);

    void negateXVelocity();

    void negateYVelocity();

    void setPosition(const sf::Vector2f& position);

    void move(const sf::Vector2f& distance);

    void setType(const DiscType& discType);

    void markDestroyed();

    const sf::Vector2f& getVelocity() const;

    const sf::Vector2f& getPosition() const;

    const DiscType& getType() const;

    bool isMarkedDestroyed() const;

    /**
     * @brief Necessary to map changes in the lib to changes in the GUI
     */
    int getId() const;

    /**
     * @returns |mv|
     */
    float getAbsoluteMomentum() const;

    /**
     * @returns 1/2*m*v^2
     */
    float getKineticEnergy() const;

private:
    /**
     * @brief Velocity in px/s
     */
    sf::Vector2f velocity_;

    /**
     * @brief Position in px
     */
    sf::Vector2f position_;

    /**
     * @brief Reactions of type A + B -> C require B to be removed (A can be changed to C). This flag
     * is set during the reaction processing and the world removed flagged discs after the update step
     */
    bool destroyed_ = false;

    /**
     * @brief The properties of this disc (mass, radius, ...)
     */
    DiscType type_;

    /**
     * @brief Used for the id
     */
    static int instanceCount;

    /**
     * @brief Unique ID for this disc
     */
    int id_;
};

#endif /* DISC_HPP */
