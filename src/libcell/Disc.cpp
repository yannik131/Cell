#include "Disc.hpp"
#include "ExceptionWithLocation.hpp"

#include <cmath>
#include <functional>
#include <stdexcept>

namespace cell
{

Disc::Disc(const DiscType& discType)
    : type_(discType)
{
}

void Disc::setVelocity(const sf::Vector2f& velocity)
{
#ifdef DEBUG
    if (std::isnan(velocity.x) || std::isnan(velocity.y) || std::isinf(velocity.x) || std::isinf(velocity.y))
        throw ExceptionWithLocation("Trying to assign an invalid value to velocity");
#endif
    velocity_ = velocity;
}

void Disc::scaleVelocity(float factor)
{
    setVelocity(velocity_ * factor);
}

void Disc::accelerate(const sf::Vector2f& acceleration)
{
    velocity_ += acceleration;
}

void Disc::negateXVelocity()
{
    velocity_.x = -velocity_.x;
}

void Disc::negateYVelocity()
{
    velocity_.y = -velocity_.y;
}

void Disc::setPosition(const sf::Vector2f& position)
{
#ifdef DEBUG
    if (std::isnan(position.x) || std::isnan(position.y) || std::isinf(position.x) || std::isinf(position.y))
        throw ExceptionWithLocation("Trying to assign an invalid value to position");
#endif
    position_ = position;
}

void Disc::move(const sf::Vector2f& distance)
{
    setPosition(position_ + distance);
}

void Disc::setType(const DiscType& discType)
{
    type_ = discType;
}

void Disc::markDestroyed()
{
    destroyed_ = true;
}

const sf::Vector2f& Disc::getVelocity() const
{
    return velocity_;
}

const sf::Vector2f& Disc::getPosition() const
{
    return position_;
}

const DiscType& Disc::getType() const
{
    return type_;
}

bool Disc::isMarkedDestroyed() const
{
    return destroyed_;
}

float Disc::getAbsoluteMomentum() const
{
    return type_.getMass() * std::hypot(velocity_.x, velocity_.y);
}

float Disc::getKineticEnergy() const
{
    return 0.5f * type_.getMass() * (velocity_.x * velocity_.x + velocity_.y * velocity_.y);
}

} // namespace cell
