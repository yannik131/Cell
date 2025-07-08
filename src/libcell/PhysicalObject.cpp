#include "PhysicalObject.hpp"
#include "ExceptionWithLocation.hpp"

#include <cmath>

namespace cell
{

void PhysicalObject::setVelocity(const sf::Vector2d& velocity)
{
#ifdef DEBUG
    if (std::isnan(velocity.x) || std::isnan(velocity.y) || std::isinf(velocity.x) || std::isinf(velocity.y))
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
    velocity_ += acceleration;
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
    if (std::isnan(position.x) || std::isnan(position.y) || std::isinf(position.x) || std::isinf(position.y))
        throw ExceptionWithLocation("Trying to assign an invalid value to position");
#endif
    position_ = position;
}

void PhysicalObject::setMass(double mass)
{
    if (mass <= 0)
        throw ExceptionWithLocation("Can't set mass to non-positive value " + std::to_string(mass));

    mass_ = mass;
}

double PhysicalObject::getMass() const
{
    return mass_;
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

double PhysicalObject::getAbsoluteMomentum() const
{
    return mass_ * std::hypot(velocity_.x, velocity_.y);
}

sf::Vector2d PhysicalObject::getMomentum() const
{
    return mass_ * velocity_;
}

double PhysicalObject::getKineticEnergy() const
{
    return 0.5f * mass_ * (velocity_.x * velocity_.x + velocity_.y * velocity_.y);
}
} // namespace cell