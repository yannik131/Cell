#ifndef ECFFF655_38C8_4C6D_AF35_3FE9EF600169_HPP
#define ECFFF655_38C8_4C6D_AF35_3FE9EF600169_HPP

#include <ostream>

namespace cell
{

struct Vector2d
{
    double x;
    double y;

    Vector2d()
        : x(0)
        , y(0)
    {
    }

    Vector2d(double x, double y)
        : x(x)
        , y(y)
    {
    }

    Vector2d& operator+=(const Vector2d& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vector2d& operator-=(const Vector2d& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vector2d& operator*=(double s) noexcept
    {
        x *= s;
        y *= s;
        return *this;
    }

    Vector2d& operator/=(double s) noexcept
    {
        x /= s;
        y /= s;
        return *this;
    }
};

inline Vector2d operator+(Vector2d lhs, const Vector2d& rhs) noexcept
{
    return lhs += rhs;
}

inline Vector2d operator-(Vector2d lhs, const Vector2d& rhs) noexcept
{
    return lhs -= rhs;
}

inline Vector2d operator*(Vector2d v, double s) noexcept
{
    return v *= s;
}

inline Vector2d operator*(double s, Vector2d v) noexcept
{
    return v *= s;
}

inline Vector2d operator/(Vector2d v, double s) noexcept
{
    return v /= s;
}

inline double operator*(Vector2d lhs, const Vector2d& rhs) noexcept
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

inline std::ostream& operator<<(std::ostream& os, const Vector2d& v)
{
    return os << "(" << v.x << ", " << v.y << ")";
}

inline Vector2d operator-(Vector2d v) noexcept
{
    return Vector2d{-v.x, -v.y};
}

} // namespace cell

#endif /* ECFFF655_38C8_4C6D_AF35_3FE9EF600169_HPP */
