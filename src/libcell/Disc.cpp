#include "Disc.hpp"

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
