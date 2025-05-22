#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include "DiscType.hpp"

#include <SFML/System/Vector2.hpp>
#include <gtest/gtest.h>

inline void expectNear(const sf::Vector2f& actual, const sf::Vector2f& expected, float epsilon)
{
    EXPECT_NEAR(actual.x, expected.x, epsilon);
    EXPECT_NEAR(actual.y, expected.y, epsilon);
}

inline DiscType discType(float radius, float mass)
{
    static int count = 0;

    char name = static_cast<int>('A') + count++ % 20;
    return DiscType{std::string(1, name), sf::Color::Green, radius, mass};
}

inline const DiscType Mass5Radius5 = discType(5, 5);
inline const DiscType Mass10Radius5 = discType(5, 10);
inline const DiscType Mass5Radius10 = discType(10, 5);
inline const DiscType Mass15Radius10 = discType(10, 15);

inline const DiscType Mass5 = discType(5, 5);
inline const DiscType Mass10 = discType(5, 10);
inline const DiscType Mass15 = discType(5, 15);
inline const DiscType Mass20 = discType(5, 20);
inline const DiscType Mass25 = discType(5, 25);
inline const DiscType Unused = discType(5, 5);

#endif /* TESTUTILS_HPP */
