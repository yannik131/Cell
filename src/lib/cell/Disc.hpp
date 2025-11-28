#ifndef EAE8DB08_087C_4EC5_AF13_54AECCF72615_HPP
#define EAE8DB08_087C_4EC5_AF13_54AECCF72615_HPP

#include "DiscType.hpp"
#include "PhysicalObject.hpp"
#include "Vector2d.hpp"

#include <SFML/System/Vector2.hpp>

namespace cell
{

/**
 * @brief Represents a particle in the simulation that can collide with others and undergo reactions. Physical
 * properties are defined by its DiscType
 */
class Disc : public PhysicalObject
{
public:
    /**
     * @brief Creates a new disc with the given type
     */
    explicit Disc(DiscTypeID discTypeID);

    /**
     * @brief Assigns a new disc type (no checks)
     */
    void setType(DiscTypeID discTypeID);

    /**
     * @brief Sets the internal destroyed flag (used for removing discs in the simulation)
     */
    void markDestroyed();

    /**
     * @returns DiscType of the disc
     */
    DiscTypeID getTypeID() const;

    /**
     * @returns `true` if `markDestroyed()` has been called
     */
    bool isMarkedDestroyed() const;

private:
    /**
     * @brief Reactions of type A + B -> C require B to be removed (A can be changed to C). This flag
     * is set during the reaction processing and the world removed flagged discs after the update step
     */
    bool destroyed_ = false;

    /**
     * @brief The properties of this disc (mass, radius, ...)
     */
    DiscTypeID discTypeID_;
};

} // namespace cell

#endif /* EAE8DB08_087C_4EC5_AF13_54AECCF72615_HPP */
