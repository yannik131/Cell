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

    struct Entry
    {
        std::size_t index;
        double radius;
        sf::Vector2d position;
        double minX, maxX;
    };

public:
    CollisionDetector(const DiscTypeRegistry& discTypeRegistry, const MembraneTypeRegistry& membraneTypeRegistry);

    RectangleCollision detectRectangularBoundsCollision(const Disc& disc, const sf::Vector2d& topLeft,
                                                        const sf::Vector2d& bottomRight) const;
    bool detectCircularBoundsCollision(const Disc& disc, const sf::Vector2d& M, double Rm) const;
    void buildEntries(const std::vector<Disc>& discs, const std::vector<Membrane>& membranes);
    std::vector<std::pair<Disc*, Disc*>> detectDiscDiscCollisions(std::vector<Disc>& discs);
    std::vector<std::pair<Membrane*, Disc*>> detectMembraneDiscCollisions(std::vector<Membrane>& membranes,
                                                                          std::vector<Disc>& discs);

    /**
     * @returns the collision counts for all disc types in the simulation and sets them to 0
     */
    DiscTypeMap<int> getAndResetCollisionCounts();

private:
    template <typename Element, typename ElementType>
    inline std::vector<CollisionDetector::Entry> createEntries(const std::vector<Element>& elements,
                                                               const TypeRegistry<ElementType>& typeRegistry) const;

private:
    DiscTypeMap<int> collisionCounts_;

    const DiscTypeRegistry& discTypeRegistry_;
    const MembraneTypeRegistry& membraneTypeRegistry_;

    std::vector<Entry> discEntries_;
    std::vector<Entry> membraneEntries_;
};

template <typename Element, typename ElementType>
inline std::vector<CollisionDetector::Entry>
CollisionDetector::createEntries(const std::vector<Element>& elements,
                                 const TypeRegistry<ElementType>& typeRegistry) const
{
    std::vector<Entry> entries;
    entries.reserve(elements.size());

    for (std::size_t i = 0; i < elements.size(); ++i)
    {
        const Element& element = elements[i];
        // Collision detection happens before reactions -> no destroyed check necessary

        const double r = typeRegistry.getByID(element.getTypeID()).getRadius();
        const auto& p = element.getPosition();
        entries.push_back(Entry{.index = i, .radius = r, .position = p, .minX = p.x - r, .maxX = p.x + r});
    }

    std::sort(entries.begin(), entries.end(), [&](const Entry& e1, const Entry& e2) { return e1.minX < e2.minX; });

    return entries;
}

} // namespace cell

#endif /* F674C74F_4648_4098_89DD_4A99F7F0CB5C_HPP */
