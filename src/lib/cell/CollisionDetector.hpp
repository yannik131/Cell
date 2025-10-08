#ifndef F674C74F_4648_4098_89DD_4A99F7F0CB5C_HPP
#define F674C74F_4648_4098_89DD_4A99F7F0CB5C_HPP

#include "Disc.hpp"
#include "Membrane.hpp"
#include "Types.hpp"
#include "Vector2d.hpp"

#include <optional>
#include <set>
#include <vector>

namespace cell
{

class CollisionDetector
{
public:
    struct RectangleCollision
    {
        enum class Wall
        {
            Left,
            Top,
            Right,
            Bottom,
            None
        };

        // A disc can either collide with 1 wall at a time or with both top/left, top/right, bottom/left and
        // bottom/right
        // We track the penetration into each wall in px
        std::optional<std::pair<Wall, double>> xCollision_;
        std::optional<std::pair<Wall, double>> yCollision_;

        bool isCollision() const
        {
            return xCollision_.has_value() || yCollision_.has_value();
        }
    };

private:
    enum class EntryType
    {
        Membrane,
        IntrudingDisc
    };

    struct Entry
    {
        std::size_t index;
        double radius;
        sf::Vector2d position;
        double minX, maxX;
    };

    struct ForeignEntry : public Entry
    {
        EntryType type_;
    };

public:
    CollisionDetector(const DiscTypeRegistry& discTypeRegistry, const MembraneTypeRegistry& membraneTypeRegistry);

    RectangleCollision detectRectangularBoundsCollision(const Disc& disc, const sf::Vector2d& topLeft,
                                                        const sf::Vector2d& bottomRight) const;
    bool detectCircularBoundsCollision(const Disc& disc, const sf::Vector2d& M, double Rm) const;
    void buildDiscEntries(const std::vector<Disc>& discs);
    void buildForeignEntries(const std::vector<Membrane>& membranes, const std::vector<Disc*>& intrudingDiscs);
    std::vector<std::pair<Disc*, Disc*>> detectDiscDiscCollisions(std::vector<Disc>& discs);
    std::vector<std::pair<Membrane*, Disc*>> detectMembraneDiscCollisions(std::vector<Membrane>& membranes,
                                                                          std::vector<Disc>& discs);

    /**
     * @returns the collision counts for all disc types in the simulation and sets them to 0
     */
    DiscTypeMap<int> getAndResetCollisionCounts();

private:
    template <typename ElementType, typename RegistryType>
    Entry createEntry(const ElementType& element, const RegistryType& registry, std::size_t index) const;

private:
    DiscTypeMap<int> collisionCounts_;

    const DiscTypeRegistry& discTypeRegistry_;
    const MembraneTypeRegistry& membraneTypeRegistry_;

    std::vector<Entry> discEntries_;
    std::vector<ForeignEntry> foreignEntries_; // Membranes and intruding discs
};

template <typename ElementType, typename RegistryType>
inline CollisionDetector::Entry CollisionDetector::createEntry(const ElementType& element, const RegistryType& registry,
                                                               std::size_t index) const
{
    const double r = registry.getByID(element.getTypeID()).getRadius();
    const auto& p = element.getPosition();
    
    return Entry{.index = index, .radius = r, .position = p, .minX = p.x - r, .maxX = p.x + r};
}

} // namespace cell

#endif /* F674C74F_4648_4098_89DD_4A99F7F0CB5C_HPP */
