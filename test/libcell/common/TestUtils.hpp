#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include "DiscType.hpp"
#include "Reaction.hpp"
#include "Vector2d.hpp"

#include <SFML/System/Vector2.hpp>
#include <gtest/gtest.h>

inline void expectNear(const sf::Vector2d& actual, const sf::Vector2d& expected, double epsilon)
{
    EXPECT_NEAR(actual.x, expected.x, epsilon);
    EXPECT_NEAR(actual.y, expected.y, epsilon);
}

inline cell::DiscType generateDiscType(double radius, double mass)
{
    static int count = 0;

    char name = static_cast<char>('A' + (count++ % 20));
    return cell::DiscType{std::string(1, name), sf::Color::Green, radius, mass};
}

inline const cell::DiscType Mass5Radius5 = generateDiscType(5, 5);
inline const cell::DiscType Mass10Radius5 = generateDiscType(5, 10);
inline const cell::DiscType Mass10Radius10 = generateDiscType(10, 10);
inline const cell::DiscType Mass5Radius10 = generateDiscType(10, 5);
inline const cell::DiscType Mass15Radius10 = generateDiscType(10, 15);

inline const cell::DiscType Mass5 = generateDiscType(5, 5);
inline const cell::DiscType Mass10 = generateDiscType(5, 10);
inline const cell::DiscType Mass15 = generateDiscType(5, 15);
inline const cell::DiscType Mass20 = generateDiscType(5, 20);
inline const cell::DiscType Mass25 = generateDiscType(5, 25);
inline const cell::DiscType Reserved = generateDiscType(5, 5);

inline auto getDefaultReactions()
{
    cell::Reaction transformation{&Mass5Radius5, nullptr, &Mass5Radius10, nullptr, 1.0};
    cell::Reaction decomposition{&Mass10, nullptr, &Mass5, &Mass5, 1.0};
    cell::Reaction combination{&Mass5, &Mass5, &Mass10, nullptr, 1.0};
    cell::Reaction exchange{&Mass5, &Mass15, &Mass10, &Mass10, 1.0};

    return std::make_tuple(transformation, decomposition, combination, exchange);
}

#endif /* TESTUTILS_HPP */
