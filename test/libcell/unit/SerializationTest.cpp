// Using nlohmann::json to serialize the settings turned out to be a little challenging, so this test suite makes sure
// that each individual class can be properly serialized

#include "DiscType.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <fstream>

using namespace cell;
using json = nlohmann::json;

TEST(SerializationTest, DiscTypeSerialization)
{
    DiscType A{"A", sf::Color::Blue, 3.5f, 1.5f};

    {
        json j = A;
        std::ofstream out("A.json");
        out << j.dump(4);
    }

    {
        json j;
        std::ifstream in("A.json");
        in >> j;
        EXPECT_EQ(A, j.get<DiscType>());
    }
}