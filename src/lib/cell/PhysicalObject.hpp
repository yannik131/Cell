#ifndef EABADE67_50B6_4B9A_933E_C1059A828B32_HPP
#define EABADE67_50B6_4B9A_933E_C1059A828B32_HPP

#include "Vector2d.hpp"

namespace cell
{

class PhysicalObject
{
public:
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
     * @returns Velocity of the disc (px/s)
     */
    const sf::Vector2d& getVelocity() const;

    /**
     * @returns Position of the disc (px)
     */
    const sf::Vector2d& getPosition() const;

    /**
     * @returns |mv|
     */
    double getAbsoluteMomentum(double mass) const;

    /**
     * @returns mv
     */
    sf::Vector2d getMomentum(double mass) const;

    /**
     * @returns 1/2*m*v^2
     */
    double getKineticEnergy(double mass) const;

private:
    bool isNanOrInf(const sf::Vector2d& vec) const;

private:
    /**
     * @brief Velocity in px/s
     */
    sf::Vector2d velocity_;

    /**
     * @brief Position in px
     */
    sf::Vector2d position_;
};

} // namespace cell

#endif /* EABADE67_50B6_4B9A_933E_C1059A828B32_HPP */
