#ifndef DISC_HPP
#define DISC_HPP

#include "PhysicalObject.hpp"
#include "types/DiscType.hpp"

#include <SFML/System/Vector2.hpp>

namespace cell
{

/**
 * @brief Represents a particle in the simulation that can collide with others and undergo reactions.
 */
class Disc : public PhysicalObject
{
public:
    /**
     * @brief Creates a new disc with the given discType and default coordinates/velocity of {0, 0}
     */
    explicit Disc(const DiscType* discType);

    /**
     * @brief Assigns a new disc type (no checks)
     */
    void setType(const DiscType* discType);

    /**
     * @brief Sets the internal destroyed flag (used for removing discs in the simulation)
     */
    void markDestroyed();

    /**
     * @returns DiscType of the disc
     */
    const DiscType* getType() const;

    /**
     * @returns `true` if `markDestroyed()` has been called
     */
    bool isMarkedDestroyed() const;

private:
    /**
     * @brief Reactions of type A + B -> C require A or B to be removed (B or A can be changed to C). This flag
     * is set during the reaction processing and the world removes flagged discs after the update step
     */
    bool destroyed_ = false;

    /**
     * @brief The properties of this disc (mass, radius, ...)
     */
    const DiscType* type_;
};

} // namespace cell

#endif /* DISC_HPP */
