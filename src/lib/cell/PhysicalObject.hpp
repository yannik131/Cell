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
    void setVelocity(const Vector2d& velocity);

    /**
     * @brief Multiplies both velocity components with `factor`
     */
    void scaleVelocity(double factor) noexcept
    {
        velocity_ *= factor;
    }

    /**
     * @brief Adds `acceleration` to the velocity of the disc
     */
    void accelerate(const Vector2d& acceleration) noexcept
    {
        velocity_ += acceleration;
    }

    /**
     * @brief Sets the position with no checks.
     * @note In debug mode, checks for invalid values (nan, inf)
     */
    void setPosition(const Vector2d& position);

    /**
     * @brief Changes the disc's position by the given `distance`
     */
    void move(const Vector2d& distance) noexcept
    {
        position_ += distance;
    }

    /**
     * @returns Velocity of the disc (px/s)
     */
    const Vector2d& getVelocity() const noexcept
    {
        return velocity_;
    }

    /**
     * @returns Position of the disc (px)
     */
    const Vector2d& getPosition() const noexcept
    {
        return position_;
    }

    /**
     * @returns |mv|
     */
    double getAbsoluteMomentum(double mass) const;

    /**
     * @returns 1/2*m*v^2
     */
    double getKineticEnergy(double mass) const noexcept
    {
        return 0.5 * mass * (velocity_.x * velocity_.x + velocity_.y * velocity_.y);
    }

private:
    bool isNanOrInf(const Vector2d& vec) const;

private:
    /**
     * @brief Velocity in px/s
     */
    Vector2d velocity_;

    /**
     * @brief Position in px
     */
    Vector2d position_;
};

} // namespace cell

#endif /* EABADE67_50B6_4B9A_933E_C1059A828B32_HPP */
