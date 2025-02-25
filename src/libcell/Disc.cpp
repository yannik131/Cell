#include "Disc.hpp"

#include <functional>

int Disc::instanceCount = 0;

Disc::Disc(const DiscType& discType)
    : type_(discType)
    , id_(instanceCount++)
{
}

int Disc::getId() const
{
    return id_;
}

float Disc::getAbsoluteMomentum() const
{
    return type_.mass_ * std::hypot(velocity_.x, velocity_.y);
}

float Disc::getKineticEnergy() const
{
    return 0.5f * type_.mass_ * velocity_.x * velocity_.x + velocity_.y * velocity_.y;
}
