#ifndef B542108E_9E93_455F_B728_C13EF16F20DA_HPP
#define B542108E_9E93_455F_B728_C13EF16F20DA_HPP

#include "cell/ExceptionWithLocation.hpp"

#include <QObject>

/**
 * @brief Performs a qobject_cast with the given type and throws and exception if it returns `nullptr`
 */
template <typename T, typename Target = QWidget> T safeCast(Target* target)
{
    T result = qobject_cast<T>(target);
    if (!result)
        throw ExceptionWithLocation("qobject_cast returned nullptr");

    return result;
}

#endif /* B542108E_9E93_455F_B728_C13EF16F20DA_HPP */
