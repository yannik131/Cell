#include "PhysicalObject.hpp"
#include "ExceptionWithLocation.hpp"
#include "MathUtils.hpp"

#include <cmath>

namespace cell
{

void PhysicalObject::setVelocity(const Vector2d& velocity)
{
#ifdef DEBUG
    if (isNanOrInf(velocity))
        throw ExceptionWithLocation("Trying to assign an invalid value to velocity");
#endif
    velocity_ = velocity;
}

void PhysicalObject::setPosition(const Vector2d& position)
{
#ifdef DEBUG
    if (isNanOrInf(position))
        throw ExceptionWithLocation("Trying to assign an invalid value to position");
#endif
    position_ = position;
}

double PhysicalObject::getAbsoluteMomentum(double mass) const
{
    return mass * std::hypot(velocity_.x, velocity_.y);
}

bool PhysicalObject::isNanOrInf(const Vector2d& vec) const
{
    return std::isnan(vec.x) || std::isnan(vec.y) || std::isinf(vec.x) || std::isinf(vec.y);
}

} // namespace cell