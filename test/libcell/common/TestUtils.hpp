#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include "cell/Disc.hpp"
#include "cell/Vector2d.hpp"

#include <gtest/gtest.h>

inline void expectNear(const cell::Vector2d& actual, const cell::Vector2d& expected, double epsilon = 1e-3)
{
    EXPECT_NEAR(actual.x, expected.x, epsilon);
    EXPECT_NEAR(actual.y, expected.y, epsilon);
}

inline std::map<std::string, int> countDiscTypes(const std::vector<cell::Disc>& discs,
                                                 const cell::DiscTypeRegistry& discTypeRegistry)
{
    std::map<std::string, int> counts;
    for (const auto& disc : discs)
        counts[discTypeRegistry.getByID(disc.getTypeID()).getName()]++;

    return counts;
}

#endif /* TESTUTILS_HPP */
