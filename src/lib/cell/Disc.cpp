#include "Disc.hpp"
#include "ExceptionWithLocation.hpp"
#include "Types.hpp"

#include <cmath>
#include <functional>
#include <stdexcept>

namespace cell
{

Disc::Disc(DiscTypeID discTypeID)
    : discTypeID_(discTypeID)
{
}

void Disc::setVelocity(const sf::Vector2d& velocity)
{
#ifdef DEBUG
    if (isNanOrInf(velocity))
        throw ExceptionWithLocation("Trying to assign an invalid value to velocity");
#endif
    velocity_ = velocity;
}

void Disc::scaleVelocity(double factor)
{
    setVelocity(velocity_ * factor);
}

void Disc::accelerate(const sf::Vector2d& acceleration)
{
    setVelocity(velocity_ + acceleration);
}

void Disc::negateXVelocity()
{
    velocity_.x = -velocity_.x;
}

void Disc::negateYVelocity()
{
    velocity_.y = -velocity_.y;
}

void Disc::setPosition(const sf::Vector2d& position)
{
#ifdef DEBUG
    if (isNanOrInf(position))
        throw ExceptionWithLocation("Trying to assign an invalid value to position");
#endif
    position_ = position;
}

void Disc::move(const sf::Vector2d& distance)
{
    setPosition(position_ + distance);
}

void Disc::setType(DiscTypeID discTypeID)
{
    discTypeID_ = discTypeID;
}

void Disc::markDestroyed()
{
    destroyed_ = true;
}

const sf::Vector2d& Disc::getVelocity() const
{
    return velocity_;
}

const sf::Vector2d& Disc::getPosition() const
{
    return position_;
}

DiscTypeID Disc::getTypeID() const
{
    return discTypeID_;
}

bool Disc::isMarkedDestroyed() const
{
    return destroyed_;
}

double Disc::getAbsoluteMomentum(const DiscTypeRegistry& discTypeRegistry) const
{
    return discTypeRegistry.getByID(discTypeID_).getMass() * std::hypot(velocity_.x, velocity_.y);
}

sf::Vector2d Disc::getMomentum(const DiscTypeRegistry& discTypeRegistry) const
{
    return discTypeRegistry.getByID(discTypeID_).getMass() * velocity_;
}

double Disc::getKineticEnergy(const DiscTypeRegistry& discTypeRegistry) const
{
    return 0.5 * discTypeRegistry.getByID(discTypeID_).getMass() *
           (velocity_.x * velocity_.x + velocity_.y * velocity_.y);
}

bool Disc::isNanOrInf(const sf::Vector2d& vec) const
{
    return std::isnan(vec.x) || std::isnan(vec.y) || std::isinf(vec.x) || std::isinf(vec.y);
}

} // namespace cell
