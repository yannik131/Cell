#ifndef TYPEBASE_HPP
#define TYPEBASE_HPP

#include <SFML/Graphics/Color.hpp>

#include <string>

namespace cell
{

class TypeBase
{
public:
    TypeBase(const TypeBase&) = delete;

    /**
     * @returns The name of this TypeBase
     */
    const std::string& getName() const;

    /**
     * @brief Sets the name for this TypeBase (can't be empty)
     */
    void setName(const std::string& name);

    /**
     * @returns The color of this TypeBase
     */
    const sf::Color& getColor() const;

    /**
     * @brief Sets the color of this TypeBase. Currently, only the predefined sf::Color static members are allowed
     * (except sf::Black because of the black simulation background)
     */
    void setColor(const sf::Color& color);

private:
    sf::Color color_;
    std::string name_;
}

} // namespace cell

#endif /* TYPEBASE_HPP */
