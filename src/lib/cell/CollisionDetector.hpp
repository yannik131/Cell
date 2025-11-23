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
    struct Params
    {
        std::vector<Disc>* discs;
        std::vector<Membrane>* membranes;
        std::vector<Disc*>* intrudingDiscs;
        Membrane* containingMembrane;
    };

private:
    enum class EntryType
    {
        Disc = 1 << 0,
        Membrane = 1 << 1,
        IntrudingDisc = 1 << 2
    };

    enum class CollisionType
    {
        DiscDisc,
        DiscContainingMembrane,
        DiscChildMembrane,
        None
    };

    struct Entry
    {
        std::size_t index;
        double radius;
        sf::Vector2d position;
        double minX, maxX;
        EntryType type;
    };

    struct Collision
    {
        int i;
        int j;
        CollisionType type;
    };

public:
    CollisionDetector(const DiscTypeRegistry& discTypeRegistry, const MembraneTypeRegistry& membraneTypeRegistry);

    void buildEntries(const std::vector<Disc>& discs, const std::vector<Membrane>& membranes,
                      const std::vector<Disc*>& intrudingDiscs);
    std::vector<Collision> detectCollisions(const Params& params);

    DiscTypeMap<int> getAndResetCollisionCounts();

private:
    template <typename ElementType, typename RegistryType>
    Entry createEntry(const ElementType& element, const RegistryType& registry, std::size_t index,
                      EntryType entryType) const;

    bool discIsContainedByMembrane(const Entry& entry);

private:
    DiscTypeMap<int> collisionCounts_;

    const DiscTypeRegistry& discTypeRegistry_;
    const MembraneTypeRegistry& membraneTypeRegistry_;

    std::vector<Entry> entries_;
    Params params_;
};

template <typename ElementType, typename RegistryType>
inline CollisionDetector::Entry CollisionDetector::createEntry(const ElementType& element, const RegistryType& registry,
                                                               std::size_t index, EntryType entryType) const
{
    const double r = registry.getByID(element.getTypeID()).getRadius();
    const auto& p = element.getPosition();

    return Entry{.index = index, .radius = r, .position = p, .minX = p.x - r, .maxX = p.x + r, .type = entryType};
}

} // namespace cell

#endif /* F674C74F_4648_4098_89DD_4A99F7F0CB5C_HPP */
