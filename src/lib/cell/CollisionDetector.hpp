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
        std::vector<Disc>* discs = nullptr;
        std::vector<Membrane>* membranes = nullptr;
        std::vector<Disc*>* intrudingDiscs = nullptr;
        Membrane* containingMembrane = nullptr;
    };

    enum class EntryType
    {
        Disc,
        Membrane,
        IntrudingDisc
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
        std::size_t index = 0;
        double radius = 0;
        sf::Vector2d position;
        double minX = 0, maxX = 0;
        EntryType type = EntryType::Disc;
    };

    struct Collision
    {
        Disc* disc = nullptr;
        Disc* otherDisc = nullptr;
        Membrane* membrane = nullptr;
        CollisionType type = CollisionType::None;
        bool allowedToPass = false; // Set in case of membrane collisions depending on permeability

        bool invalidatedByDestroyedDiscs() const
        {
            return disc->isMarkedDestroyed() || (otherDisc && otherDisc->isMarkedDestroyed());
        }
    };

private:
    struct EntryComparator
    {
        bool operator()(const Entry& e1, const Entry& e2) const
        {
            return e1.minX < e2.minX;
        }
    } entryComparator_;

public:
    CollisionDetector(const DiscTypeRegistry& discTypeRegistry, const MembraneTypeRegistry& membraneTypeRegistry);
    void setParams(Params params);
    void buildMembraneIndex();
    void buildDiscIndex();
    void addIntrudingDiscsToIndex();

    std::vector<Collision> detectDiscMembraneCollisions();
    std::vector<Collision> detectDiscDiscCollisions();

    static DiscTypeMap<int> getAndResetCollisionCounts();

private:
    template <typename ElementType, typename RegistryType>
    Entry createEntry(const ElementType& element, const RegistryType& registry, std::size_t index,
                      EntryType entryType) const;

    bool discIsContainedByMembrane(const Entry& entry);
    bool canGoThrough(Disc* disc, Membrane* membrane, CollisionDetector::CollisionType collisionType) const;

private:
    static DiscTypeMap<int> collisionCounts_;

    const DiscTypeRegistry& discTypeRegistry_;
    const MembraneTypeRegistry& membraneTypeRegistry_;

    std::vector<Entry> membraneEntries_;
    std::vector<Entry> discEntries_;
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
