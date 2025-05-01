#ifndef EXCEPTIONMACRO_HPP
#define EXCEPTIONMACRO_HPP

/**
 * @brief Helper macro to easily define custom exception types
 */
#define CUSTOM_EXCEPTION(NAME)                                                                                         \
    class NAME : public std::runtime_error                                                                             \
    {                                                                                                                  \
    public:                                                                                                            \
        explicit NAME(const std::string& message)                                                                      \
            : std::runtime_error(message)                                                                              \
        {                                                                                                              \
        }                                                                                                              \
    };

#endif /* EXCEPTIONMACRO_HPP */
