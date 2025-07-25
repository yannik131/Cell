#ifndef COLLISIONDETECTOR_HPP
#define COLLISIONDETECTOR_HPP

#include "PositionNanoflannAdapter.hpp"
#include "Vector2d.hpp"

#include <nanoflann.hpp>

#include <optional>
#include <set>
#include <utility>
#include <vector>

namespace cell
{

class Disc;
class Membrane;

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
        // bottom/right We track the penetration into each wall in px
        std::optional<std::pair<Wall, double>> xCollision_;
        std::optional<std::pair<Wall, double>> yCollision_;
    };

    struct MembranePolygonPoint
    {
        sf::Vector2d position_;
        const Membrane* membrane_;

        const sf::Vector2d& getPosition() const
        {
            return position_;
        }
    };

    template <typename T> using AdapterType = nanoflann::L2_Simple_Adaptor<double, PositionNanoflannAdapter<T>>;

    template <typename T>
    using KDTree = nanoflann::KDTreeSingleIndexAdaptor<AdapterType<T>, PositionNanoflannAdapter<T>, 2>;

public:
    CollisionDetector();

    void buildMembraneKdTree(const std::vector<Membrane>& membranes);

    /**
     * @brief Finds all discs and membranes in the given vectors that overlap using nanoflann. Note that this uses the
     * maximum radius in the disc type distribution from the settings
     * @param discs The vector of discs
     */
    void detectCollisions(std::vector<Disc>& discs, std::vector<Membrane>& membranes);

    static RectangleCollision detectDiscRectangleCollision(const Disc& disc, const sf::Vector2d& topLeft,
                                                           const sf::Vector2d& bottomRight);

    void updateMaxRadii();

    double getMaxDiscRadius() const;

    std::set<std::pair<Disc*, Disc*>> getDiscDiscCollisions() const;
    std::set<std::pair<Disc*, Membrane*>> getDiscMembraneCollisions() const;

private:
    std::set<std::pair<Disc*, Disc*>> detectDiscDiscCollisions(std::vector<Disc>& discs);
    std::set<std::pair<Disc*, Membrane*>> detectDiscMembraneCollisions(std::vector<Disc>& discs,
                                                                       std::vector<Membrane>& membranes);

private:
    std::set<std::pair<Disc*, Disc*>> discDiscCollisions_;
    std::set<std::pair<Disc*, Membrane*>> discMembraneCollisions_;
    double maxDiscRadius_ = 0;
    double minDiscRadius_ = 0;
    double maxMembraneRadius_ = 0;
    std::vector<MembranePolygonPoint> membranePolygonPoints_;
    KDTree<MembranePolygonPoint> membranesKDTree;                  // naming
    const PositionNanoflannAdapter<MembranePolygonPoint> adapter_; // naming
};

} // namespace cell

#endif /* COLLISIONDETECTOR_HPP */
