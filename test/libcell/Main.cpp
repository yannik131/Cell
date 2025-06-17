#include "Logging.hpp"

#include <glog/logging.h>
#include <gtest/gtest.h>

#include <filesystem>

bool cdUpUntilInTestDir()
{
    namespace fs = std::filesystem;

    for (int i = 0; i < 3; ++i)
    {
        if (fs::exists("test/output") && fs::is_directory("test/output"))
        {
            fs::current_path("test/output");
            return true;
        }
        fs::current_path("..");
    }

    LOG(ERROR) << "Folder 'test/output' not found after 3 attempts.";

    return false;
}

/**
 * We need the main function to configure logging
 */
int main(int argc, char** argv)
{
    cell::initLogging(argc, argv);
    if (!cdUpUntilInTestDir())
        return 1;

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}