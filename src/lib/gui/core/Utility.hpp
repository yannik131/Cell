#ifndef DCDB89B2_8FE4_4EB9_A8EA_6C13300ADEBA_HPP
#define DCDB89B2_8FE4_4EB9_A8EA_6C13300ADEBA_HPP

#include "delegates/ComboBoxDelegate.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <QMessageBox>

namespace utility
{

/**
 * @brief Entry-wise addition of 2 maps, i. e. {1: 1, 2: 3} + {1: 0, 2: 4} = {1: 1, 2: 7}
 */
template <typename T1, typename T2, typename T3, typename T4>
void addMaps(std::unordered_map<T1, T2, T3, T4>& a, const std::unordered_map<T1, T2, T3, T4>& b)
{
    for (const auto& [key, value] : b)
        a[key] += value;
}

/**
 * @brief Entry-wise division of 2 maps
 */
template <typename T1, typename T2, typename T3, typename T4, typename T5>
void divideValuesBy(std::unordered_map<T1, T2, T3, T5>& a, const T4& b)
{
    for (const auto& [key, value] : a)
        a[key] /= b;
}

/*template <typename T1, typename T2, typename T3, typename T4, typename T5>
std::unordered_map<T1, T2, T3, T5> multiplyValuesBy(std::unordered_map<T1, T2, T3, T5> a, const T4& b)
{
    for (const auto& [key, value] : a)
        a[key] *= b;

    return a;
}*/

/**
 * @brief Converts a sfml color to a QColor
 */
QColor sfColorToQColor(const sf::Color& sfColor);

sf::Vector2f toVector2f(const cell::Vector2d& v);

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
