#ifndef EXCEPTIONWITHLOCATION_HPP
#define EXCEPTIONWITHLOCATION_HPP

#include <source_location>
#include <sstream>
#include <stdexcept>

class ExceptionWithLocation : public std::runtime_error
{
public:
    ExceptionWithLocation(const std::string& description,
                          const std::source_location location = std::source_location::current())
        : std::runtime_error(buildMessage(description, location))
    {
    }

private:
    static std::string buildMessage(const std::string& description, const std::source_location& location)
    {
        std::ostringstream oss;
        oss << "Error:\t" << description << "\nLine:\t" << location.file_name() << ":" << location.line()
            << "\nFunction:\t" << location.function_name();
        return oss.str();
    }
};

#endif /* EXCEPTIONWITHLOCATION_HPP */
