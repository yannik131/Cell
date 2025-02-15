#include "DiscType.hpp"

bool operator==(const DiscType& a, const DiscType& b)
{
    return a.name_ == b.name_;
}

bool operator<(const DiscType& a, const DiscType& b)
{
    return a.name_ < b.name_;
}
