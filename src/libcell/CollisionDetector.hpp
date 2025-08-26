#ifndef COLLISIONDETECTOR_HPP
#define COLLISIONDETECTOR_HPP

#include "PositionNanoflannAdapter.hpp"
#include "Types.hpp"
#include "Vector2d.hpp"

#include <nanoflann.hpp>

#include <optional>
#include <set>
#include <vector>

namespace cell
{

class Disc;

class CollisionDetector
{
public:
    template <typename T> using AdapterType = nanoflann::L2_Simple_Adaptor<double, PositionNanoflannAdapter<T>>;

    template <typename T>
    using KDTree = nanoflann::KDTreeSingleIndexAdaptor<AdapterType<T>, PositionNanoflannAdapter<T>, 2>;

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

public:
    CollisionDetector(DiscTypeResolver discTypeResolver, MaxRadiusProvider maxRadiusProvider);

    void detectCollisions(std::vector<Disc>& discs);

    RectangleCollision detectDiscRectangleCollision(const Disc& disc, const sf::Vector2d& topLeft,
                                                    const sf::Vector2d& bottomRight) const;

    std::set<std::pair<Disc*, Disc*>> getDiscDiscCollisions() const;

    std::set<std::pair<Disc*, Disc*>> detectDiscDiscCollisions(std::vector<Disc>& discs) const;

private:
    std::set<std::pair<Disc*, Disc*>> discDiscCollisions_;
    DiscTypeResolver discTypeResolver_;
    MaxRadiusProvider maxRadiusProvider_;
};

} // namespace cell

#endif /* COLLISIONDETECTOR_HPP */
