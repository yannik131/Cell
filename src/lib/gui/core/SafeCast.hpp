#ifndef SAFECAST_HPP
#define SAFECAST_HPP

#include "cell/ExceptionWithLocation.hpp"

#include <QObject>

/**
 * @brief Performs a qobject_cast with the given type and throws and exception if it returns `nullptr`
 */
template <typename T> T safeCast(QWidget* widget)
{
    T result = qobject_cast<T>(widget);
    if (!result)
        throw ExceptionWithLocation("qobject_cast returned nullptr");

    return result;
}

#endif /* SAFECAST_HPP */
