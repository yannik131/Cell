#ifndef DISC_HPP
#define DISC_HPP

#include "DiscType.hpp"
#include "JsonSerializers.hpp"

#include <SFML/System/Vector2.hpp>
#include <nlohmann/json.hpp>

namespace cell
{

/**
 * @brief Represents a particle in the simulation that can collide with others and undergo reactions. Physical
 * properties are defined by its DiscType
 */
class Disc
{
public:
    /**
     * @brief Copy-ctor that does not affect id
     */
    explicit Disc(const DiscType& discType);

    /**
     * @brief Sets the velocity of the disc in px/s
     */
    void setVelocity(const sf::Vector2f& velocity);

    /**
     * @brief Multiplies both velocity components with `factor`
     */
    void scaleVelocity(float factor);

    /**
     * @brief Adds `acceleration` to the velocity of the disc
     */
    void accelerate(const sf::Vector2f& acceleration);

    /**
     * @brief Negates the x-component of the velocity
     */
    void negateXVelocity();

    /**
     * @brief Negates the y-component of the velocity
     */
    void negateYVelocity();

    /**
     * @brief Sets the position with no checks.
     * @note In debug mode, checks for invalid values (nan, inf)
     */
    void setPosition(const sf::Vector2f& position);

    /**
     * @brief Changes the disc's position by the given `distance`
     */
    void move(const sf::Vector2f& distance);

    /**
     * @brief Assigns a new disc type (no checks)
     */
    void setType(const DiscType& discType);

    /**
     * @brief Sets the internal destroyed flag (used for removing discs in the simulation)
     */
    void markDestroyed();

    /**
     * @returns Velocity of the disc (px/s)
     */
    const sf::Vector2f& getVelocity() const;

    /**
     * @returns Position of the disc (px)
     */
    const sf::Vector2f& getPosition() const;

    /**
     * @returns DiscType of the disc
     */
    const DiscType& getType() const;

    /**
     * @returns `true` if `markDestroyed()` has been called
     */
    bool isMarkedDestroyed() const;

    /**
     * @brief Id using the instance count. Necessary to map changes in the lib to changes in the GUI
     */
    int getId() const;

    /**
     * @returns |mv|
     */
    float getAbsoluteMomentum() const;

    /**
     * @returns 1/2*m*v^2
     */
    float getKineticEnergy() const;

private:
    /**
     * @brief Velocity in px/s
     */
    sf::Vector2f velocity_;

    /**
     * @brief Position in px
     */
    sf::Vector2f position_;

    /**
     * @brief Reactions of type A + B -> C require B to be removed (A can be changed to C). This flag
     * is set during the reaction processing and the world removed flagged discs after the update step
     */
    bool destroyed_ = false;

    /**
     * @brief The properties of this disc (mass, radius, ...)
     */
    DiscType type_;

    /**
     * @brief Used for the id
     */
    static int instanceCount;

    /**
     * @brief Unique ID for this disc
     */
    int id_;

public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Disc, velocity_, position_, destroyed_, type_, id_)
};

} // namespace cell

#endif /* DISC_HPP */
