#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include <SFML/System/Vector2.hpp>
#include <gtest/gtest.h>

inline void expectNear(const sf::Vector2f& actual, const sf::Vector2f& expected, float epsilon)
{
    EXPECT_NEAR(actual.x, expected.x, epsilon);
    EXPECT_NEAR(actual.y, expected.y, epsilon);
}

#endif /* TESTUTILS_HPP */
