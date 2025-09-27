#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include "cell/DiscType.hpp"
#include "cell/Reaction.hpp"
#include "cell/Vector2d.hpp"

#include <SFML/System/Vector2.hpp>
#include <gtest/gtest.h>

inline void expectNear(const sf::Vector2d& actual, const sf::Vector2d& expected, double epsilon = 1e-3)
{
    EXPECT_NEAR(actual.x, expected.x, epsilon);
    EXPECT_NEAR(actual.y, expected.y, epsilon);
}

#endif /* TESTUTILS_HPP */
