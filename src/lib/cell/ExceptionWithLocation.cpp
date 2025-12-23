#include "ExceptionWithLocation.hpp"

#include <sstream>

ExceptionWithLocation::ExceptionWithLocation(const std::string& description, const std::source_location location)
    : std::runtime_error(buildMessage(description, location))
{
}

std::string ExceptionWithLocation::buildMessage(const std::string& description,
                                                [[maybe_unused]] const std::source_location& location)
{
    std::ostringstream oss;
    oss << "Error: " << description;

#ifdef DEBUG
    oss << "\nLine: " << fs::path(location.file_name()).filename().string() << ":" << location.line()
        << "\nFunction: " << location.function_name();
#endif

    return oss.str();
}