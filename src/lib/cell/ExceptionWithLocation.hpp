#ifndef EXCEPTIONWITHLOCATION_HPP
#define EXCEPTIONWITHLOCATION_HPP

#include <filesystem>
#include <source_location>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

/**
 * @brief This is really just an experiment to see if giving additional information for exceptions is useful when
 * debugging because typically I just copy-paste the exception message and strg-f for it in the project
 * @note So far not really a big difference
 */
class ExceptionWithLocation : public std::runtime_error
{
public:
    ExceptionWithLocation(const std::string& description,
                          const std::source_location location = std::source_location::current());

private:
    static std::string buildMessage(const std::string& description, const std::source_location& location);
};

#endif /* EXCEPTIONWITHLOCATION_HPP */
