#ifndef EXCEPTIONMACRO_HPP
#define EXCEPTIONMACRO_HPP

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
