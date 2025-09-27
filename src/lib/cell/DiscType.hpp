#ifndef CFE848F0_38AC_4182_9A69_D7C2AB0577CA_HPP
#define CFE848F0_38AC_4182_9A69_D7C2AB0577CA_HPP

#include "Hashing.hpp"
#include "Types.hpp"

#include <SFML/Graphics/Color.hpp>

#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace cell
{

/**
 * @brief Contains the physical and visual properties of a disc. Disc types used in the simulation need to be distinct
 * by name
 */
class DiscType
{
public:
    /**
     * @brief Creates a new disc type
     */
    DiscType(const std::string& name, Radius radius, Mass mass);

    /**
     * @brief Disc types should be shared across all discs and be unique
     */
    DiscType(const DiscType&) = delete;
    DiscType& operator=(const DiscType&) = delete;
    DiscType(DiscType&&) = default;
    DiscType& operator=(DiscType&&) = default;

    /**
     * @returns The name of this DiscType
     */
    const std::string& getName() const;

    /**
     * @brief Sets the name for this DiscType (can't be empty)
     */
    void setName(const std::string& name);

    /**
     * @returns The radius of this DiscType in px
     */
    double getRadius() const;

    /**
     * @brief Sets the radius of this DiscType in px (must be > 0)
     */
    void setRadius(double radius);

    /**
     * @returns The mass of this DiscType (arbitrary unit right now)
     */
    double getMass() const;

    /**
     * @brief Sets the mass of this DiscType (must be > 0, arbitrary unit)
     */
    void setMass(double mass);

    /**
     * @brief Comparison by all members
     */
    bool operator==(const DiscType& other) const;

private:
    /**
     * @brief The name is used to uniquely identify disc types within the disc type distribution
     */
    std::string name_;

    /**
     * @brief Radius in px
     */
    double radius_ = 0;

    /**
     * @brief Mass in arbitrary unit
     */
    double mass_ = 0;
};

} // namespace cell

#endif /* CFE848F0_38AC_4182_9A69_D7C2AB0577CA_HPP */
