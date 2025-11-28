#include "PhysicalObject.hpp"
#include "ExceptionWithLocation.hpp"

#include <cmath>

namespace cell
{

void PhysicalObject::setVelocity(const sf::Vector2d& velocity)
{
#ifdef DEBUG
    if (isNanOrInf(velocity))
        throw ExceptionWithLocation("Trying to assign an invalid value to velocity");
#endif
    velocity_ = velocity;
}

void PhysicalObject::scaleVelocity(double factor)
{
    setVelocity(velocity_ * factor);
}

void PhysicalObject::accelerate(const sf::Vector2d& acceleration)
{
    setVelocity(velocity_ + acceleration);
}

void PhysicalObject::negateXVelocity()
{
    velocity_.x = -velocity_.x;
}

void PhysicalObject::negateYVelocity()
{
    velocity_.y = -velocity_.y;
}

void PhysicalObject::setPosition(const sf::Vector2d& position)
{
#ifdef DEBUG
    if (isNanOrInf(position))
        throw ExceptionWithLocation("Trying to assign an invalid value to position");
#endif
    position_ = position;
}

void PhysicalObject::move(const sf::Vector2d& distance)
{
    setPosition(position_ + distance);
}

const sf::Vector2d& PhysicalObject::getVelocity() const
{
    return velocity_;
}

const sf::Vector2d& PhysicalObject::getPosition() const
{
    return position_;
}

double PhysicalObject::getAbsoluteMomentum(double mass) const
{
    return mass * std::hypot(velocity_.x, velocity_.y);
}

sf::Vector2d PhysicalObject::getMomentum(double mass) const
{
    return mass * velocity_;
}

double PhysicalObject::getKineticEnergy(double mass) const
{
    return 0.5 * mass * (velocity_.x * velocity_.x + velocity_.y * velocity_.y);
}

bool PhysicalObject::isNanOrInf(const sf::Vector2d& vec) const
{
    return std::isnan(vec.x) || std::isnan(vec.y) || std::isinf(vec.x) || std::isinf(vec.y);
}

} // namespace cell