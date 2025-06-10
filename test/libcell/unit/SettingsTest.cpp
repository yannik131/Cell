#include "Settings.hpp"

#include <gtest/gtest.h>

#include <SFML/System/Time.hpp>

TEST(SettingsTest, SettingsAreWrittenAndReadCorrectly)
{
    {
        cell::Settings settings;

        settings.set("i", 42, 20, 100);
        settings.set("f", 42.2f, 3.f, 50.f);
        settings.set("d", 34.2, std::nullopt, 100);
        settings.set("c", 'g');
        settings.set("b", true);
        settings.set("s", std::string("hello world"));
        settings.set("t", sf::microseconds(123), sf::microseconds(50), std::nullopt);

        settings.saveAsJson("settings.json");
    }

    cell::Settings settings;
    settings.loadFromJson("settings.json");

    EXPECT_EQ(settings.get<int>("i"), 42);
    EXPECT_EQ(settings.getMin<int>("i"), 20);
    EXPECT_EQ(settings.getMax<int>("i"), 100);

    EXPECT_EQ(settings.get<float>("f"), 42.2f);
    EXPECT_EQ(settings.getMin<float>("f"), 3.f);
    EXPECT_EQ(settings.getMax<float>("f"), 50.f);

    EXPECT_EQ(settings.get<double>("d"), 34.2);
    EXPECT_EQ(settings.get<char>("c"), 'c');
    EXPECT_EQ(settings.get<bool>("b"), true);
    EXPECT_EQ(settings.get<std::string>("s"), "hello world");
    EXPECT_EQ(settings.get<sf::Time>("t"), sf::microseconds(123));
}