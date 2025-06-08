#ifndef SAFECAST_HPP
#define SAFECAST_HPP

#include "ExceptionWithLocation.hpp"

#include <QObject>

template <typename T> T safeCast(QWidget* widget)
{
    T result = qobject_cast<T>(widget);
    if (!result)
        throw ExceptionWithLocation("qobject_cast returned nullptr");

    return result;
}

#endif /* SAFECAST_HPP */
