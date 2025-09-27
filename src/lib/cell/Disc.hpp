#ifndef EAE8DB08_087C_4EC5_AF13_54AECCF72615_HPP
#define EAE8DB08_087C_4EC5_AF13_54AECCF72615_HPP

#include "DiscType.hpp"
#include "Vector2d.hpp"

#include <SFML/System/Vector2.hpp>

namespace cell
{

/**
 * @brief Represents a particle in the simulation that can collide with others and undergo reactions. Physical
 * properties are defined by its DiscType
 */
class Disc
{
public:
    /**
     * @brief Creates a new disc with the given type
     */
    explicit Disc(DiscTypeID discTypeID);

    /**
     * @brief Sets the velocity of the disc in px/s
     */
    void setVelocity(const sf::Vector2d& velocity);

    /**
     * @brief Multiplies both velocity components with `factor`
     */
    void scaleVelocity(double factor);

    /**
     * @brief Adds `acceleration` to the velocity of the disc
     */
    void accelerate(const sf::Vector2d& acceleration);

    /**
     * @brief Negates the x-component of the velocity
     */
    void negateXVelocity();

    /**
     * @brief Negates the y-component of the velocity
     */
    void negateYVelocity();

    /**
     * @brief Sets the position with no checks.
     * @note In debug mode, checks for invalid values (nan, inf)
     */
    void setPosition(const sf::Vector2d& position);

    /**
     * @brief Changes the disc's position by the given `distance`
     */
    void move(const sf::Vector2d& distance);

    /**
     * @brief Assigns a new disc type (no checks)
     */
    void setType(DiscTypeID discTypeID);

    /**
     * @brief Sets the internal destroyed flag (used for removing discs in the simulation)
     */
    void markDestroyed();

    /**
     * @returns Velocity of the disc (px/s)
     */
    const sf::Vector2d& getVelocity() const;

    /**
     * @returns Position of the disc (px)
     */
    const sf::Vector2d& getPosition() const;

    /**
     * @returns DiscType of the disc
     */
    DiscTypeID getDiscTypeID() const;

    /**
     * @returns `true` if `markDestroyed()` has been called
     */
    bool isMarkedDestroyed() const;

    // TODO Move physics stuff into own base class PhysicalObject
    /**
     * @returns |mv|
     */
    double getAbsoluteMomentum(const DiscTypeRegistry& discTypeResolver) const;

    /**
     * @returns mv
     */
    sf::Vector2d getMomentum(const DiscTypeRegistry& discTypeResolver) const;

    /**
     * @returns 1/2*m*v^2
     */
    double getKineticEnergy(const DiscTypeRegistry& discTypeResolver) const;

private:
    /**
     * @brief Velocity in px/s
     */
    sf::Vector2d velocity_;

    /**
     * @brief Position in px
     */
    sf::Vector2d position_;

    /**
     * @brief Reactions of type A + B -> C require B to be removed (A can be changed to C). This flag
     * is set during the reaction processing and the world removed flagged discs after the update step
     */
    bool destroyed_ = false;

    /**
     * @brief The properties of this disc (mass, radius, ...)
     */
    DiscTypeID discTypeID_;
};

} // namespace cell

#endif /* EAE8DB08_087C_4EC5_AF13_54AECCF72615_HPP */
