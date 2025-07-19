#ifndef PHYSICALOBJECT_HPP
#define PHYSICALOBJECT_HPP

#include "Vector2d.hpp"

namespace cell
{

/**
 * @brief A physical object with mass, position, velocity etc.
 */
class PhysicalObject
{
public:
    /**
     * @brief Sets the velocity in px/s
     */
    void setVelocity(const sf::Vector2d& velocity);

    /**
     * @brief Multiplies both velocity components with `factor`
     */
    void scaleVelocity(double factor);

    /**
     * @brief Adds `acceleration` to the velocity
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
     * @brief Sets the dimensionless mass for this object
     */
    void setMass(double mass);

    /**
     * @returns dimensionless mass
     */
    double getMass() const;

    /**
     * @brief Changes the disc's position by the given `distance`
     */
    void move(const sf::Vector2d& distance);

    /**
     * @returns Velocity (px/s)
     */
    const sf::Vector2d& getVelocity() const;

    /**
     * @returns Position (px)
     */
    const sf::Vector2d& getPosition() const;

    /**
     * @returns |mv|
     */
    double getAbsoluteMomentum() const;

    /**
     * @returns mv
     */
    sf::Vector2d getMomentum() const;

    /**
     * @returns 1/2*m*v^2
     */
    double getKineticEnergy() const;

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
     * @brief Dimensionless mass
     */
    double mass_;
};
} // namespace cell

#endif /* PHYSICALOBJECT_HPP */
