#include "Disc.hpp"

#include <gtest/gtest.h>

static cell::DiscType defaultType()
{
    return {"A", sf::Color::White, 5.f, 5.f};
}

static const cell::DiscType Type = defaultType();

TEST(DiscTest, SetTypeUpdatesDiscType)
{
    cell::DiscType newType{"B", sf::Color::Green, 4.f, 4.f};

    cell::Disc disc(Type);
    disc.setType(newType);

    EXPECT_EQ(disc.getType(), newType);
}

TEST(DiscTest, MarkDestroyedSetsFlag)
{
    cell::Disc disc(Type);
    disc.markDestroyed();

    EXPECT_TRUE(disc.isMarkedDestroyed());
}
