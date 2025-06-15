#include "Logging.hpp"

#include <gtest/gtest.h>

#include <filesystem>

void cdUpUntilInTestDir()
{
    namespace fs = std::filesystem;

    for (int i = 0; i < 3; ++i)
    {
        if (fs::exists("test/output") && fs::is_directory("test/output"))
        {
            fs::current_path("test/output");
            return;
        }
        fs::current_path("..");
    }

    throw std::runtime_error("Folder 'test/output' not found after 3 attempts.");
}

/**
 * We need the main function to configure logging
 */
int main(int argc, char** argv)
{
    cdUpUntilInTestDir();
    testing::InitGoogleTest(&argc, argv);
    cell::initLogging(argc, argv);
    return RUN_ALL_TESTS();
}