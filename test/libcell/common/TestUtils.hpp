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

#endif /* TESTUTILS_HPP */
