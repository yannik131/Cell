#include "Hashing.hpp"

#include <gmock/gmock.h>

#include <unordered_set>

TEST(AHashFunction, CalculatesDifferentHashesForDifferentPoints)
{
    std::unordered_set<std::size_t> hashSet;

    for (int x = 0; x <= 100; ++x)
    {
        for (int y = 0; y <= 100; ++y)
        {
            auto hashValue = cell::calculateHash(x, y);
            if (hashSet.find(hashValue) != hashSet.end())
                FAIL() << "Duplicate hash value for " << x << " " << y;

            hashSet.insert(hashValue);
        }
    }
}