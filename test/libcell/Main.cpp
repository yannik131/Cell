#include "Logging.hpp"

#include <gtest/gtest.h>

/**
 * We need the main function to configure logging
 */
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    cell::initLogging(argc, argv);
    return RUN_ALL_TESTS();
}