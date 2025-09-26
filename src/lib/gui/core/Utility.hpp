#ifndef DCDB89B2_8FE4_4EB9_A8EA_6C13300ADEBA_HPP
#define DCDB89B2_8FE4_4EB9_A8EA_6C13300ADEBA_HPP

#include "delegates/ComboBoxDelegate.hpp"

#include <SFML/Graphics/Color.hpp>

#include <QMessageBox>

namespace utility
{

/**
 * @brief Converts a sfml color to a QColor
 */
QColor sfColorToQColor(const sf::Color& sfColor);

/**
 * @brief Turns a QMap<key, value> into a map<value, key>
 */
template <template <typename, typename> class MapType, typename KeyType, typename ValueType>
MapType<ValueType, KeyType> invertMap(const MapType<KeyType, ValueType>& map)
{
    MapType<ValueType, KeyType> inverted;

    for (auto it = map.begin(); it != map.end(); ++it)
        inverted[it.value()] = it.key();

    return inverted;
}

/**
 * @returns A callable that executes the given callable. If the given callable `f` throws, catches the exception and
 * displays it using QMessageBox. Forwards any arguments to the callable or calls it without them if it doesn't take
 * any.
 */
template <typename F> auto safeSlot(QWidget* parent, F&& f)
{
    return [parent, fun = std::forward<F>(f)](auto&&... args)
    {
        try
        {
            if constexpr (std::is_invocable_v<F, decltype(args)...>)
                fun(std::forward<decltype(args)>(args)...);
            else if constexpr (std::is_invocable_v<F>)
                fun();
        }
        catch (const std::exception& e)
        {
            QMessageBox::warning(parent, QStringLiteral("Epic fail!"), QString::fromUtf8(e.what()));
        }
        catch (...)
        {
            QMessageBox::warning(parent, QStringLiteral("Epic unknown fail!"),
                                 QStringLiteral("Unknown error occurred while executing callback!"));
        }
    };
}

}; // namespace utility

#endif /* DCDB89B2_8FE4_4EB9_A8EA_6C13300ADEBA_HPP */
