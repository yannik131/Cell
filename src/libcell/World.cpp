#include "World.hpp"
#include "NanoflannAdapter.hpp"

#include <glog/logging.h>

#include <map>
#include <random>
#include <set>
#include <cmath>
#include <algorithm>

World::World()
{
    maxRadius_ = std::max_element(RadiusDistribution_.begin(), RadiusDistribution_.end(),
                                     [](const auto& a, const auto& b) { return a.second < b.second; })
                        ->second;
}

void World::update(const sf::Time& dt)
{
    for (auto& disc : discs_)
    {
        disc.position_ += disc.velocity_ * dt.asSeconds();
        const auto& collidingDiscs = findCollidingDiscs();
        handleDiscCollisions(collidingDiscs, dt);
        handleWorldBoundCollision(disc);
    }
}

int World::getAndResetCollisionCount()
{
    int tmp = collisionCount_;
    collisionCount_ = 0;

    return tmp;
}

const std::vector<Disc>& World::discs() const
{
    return discs_;
}

void World::reset()
{
    discs_.clear();
    startPositions_.clear();

    initializeStartPositions();
    buildScene();
}

void World::setNumberOfDiscs(int numberOfDiscs)
{
    if(numberOfDiscs <= 0 || numberOfDiscs > 500)
        throw std::runtime_error("Number of discs must be between 1 and 500");

    numberOfDiscs_ = numberOfDiscs;
}

void World::setBounds(const sf::Vector2f& bounds)
{
    if(bounds.x <= 0 || bounds.y <= 0)
        throw std::runtime_error("Bounds must be > 0");

    bounds_ = bounds;
}

void World::buildScene()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution(0, 1);
    std::uniform_int_distribution<int> velocityDistribution(-600, 600);

    discs_.reserve(numberOfDiscs_);
    std::map<int, int> counts;

    for (int i = 0; i < numberOfDiscs_ && !startPositions_.empty(); ++i)
    {
        float randomNumber = distribution(gen);

        for (const auto& [probability, radius] : RadiusDistribution_)
        {
            if (randomNumber < probability)
            {
                counts[radius]++;
                Disc newDisc(radius);
                newDisc.position_ = startPositions_.back();
                newDisc.velocity_ = sf::Vector2f(velocityDistribution(gen), velocityDistribution(gen));

                discs_.push_back(newDisc);
                startPositions_.pop_back();
                
                break;
            }
        }
    }
    
    VLOG(1) << "Radius distribution";
    for(const auto& [radius, count] : counts) {
        VLOG(1) << radius << ": " << count << "/" << numberOfDiscs_ << " (" << count / static_cast<float>(numberOfDiscs_) * 100 << "%)\n";
    }
}

void World::initializeStartPositions()
{
    if(bounds_.x == 0 || bounds_.y == 0)
        throw std::runtime_error("Can't initialize world: Bounds not set");

    startPositions_.reserve(numberOfDiscs_);

    float spacing = maxRadius_ + 1;

    for(float x = spacing; x < bounds_.x - spacing; x += 2*spacing) {
        for(float y = spacing; y < bounds_.y - spacing; y += 2*spacing)
            startPositions_.push_back(sf::Vector2f(x, y));
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(startPositions_.begin(), startPositions_.end(), g);
}

void World::handleWorldBoundCollision(Disc& disc)
{
    //https://hermann-baum.de/bouncing-balls/
    const auto& r = disc.radius_;
    auto& pos = disc.position_;
    auto& v = disc.velocity_;

    float dx, dy;
    
    if (pos.x < r)
    {
        dx = r - pos.x + 1;
        dy = dx * v.y / v.x;
        
        pos += {dx, dy};
        v.x = -v.x;
    }
    else if (pos.x > bounds_.x - r)
    {
        dx = -(pos.x + r - bounds_.x + 1);
        dy = -(dx * v.y / v.x);
        
        pos += {dx, dy};
        v.x = -v.x;
    }

    if (pos.y < r)
    {
        dy = r - pos.y + 1;
        dx = dy * v.x / v.y;
        
        pos += {dx, dy};
        v.y = -v.y;
    }
    else if (pos.y > bounds_.y - r)
    {
        dy = -(pos.y + r - bounds_.y + 1);
        dx = -(dy * v.x / v.y);

        pos += {dx, dy};
        v.y = -v.y;
    }
}

std::set<std::pair<Disc*, Disc*>> World::findCollidingDiscs()
{
    NanoflannAdapter adapter(discs_);
    typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, NanoflannAdapter>, NanoflannAdapter,
                                                2>
        KDTree;
    KDTree kdtree(2, adapter);

    std::set<std::pair<Disc*, Disc*>> collidingDiscs;

    for (auto& disc : discs_)
    {
        std::vector<nanoflann::ResultItem<uint32_t, float>> results;
        const float maxCollisionDistance = disc.radius_ + maxRadius_;

        kdtree.radiusSearch(&disc.position_.x, maxCollisionDistance * maxCollisionDistance, results);

        for (size_t i = 1; i < results.size(); ++i)
        {
            auto& otherDisc = discs_[results[i].first];
            const float radiusSum = disc.radius_ + otherDisc.radius_;

            if (results[i].second <= radiusSum * radiusSum) {
                auto p1 = &disc;
                auto p2 = &otherDisc;
                if(p2 < p1) std::swap(p1, p2);
                collidingDiscs.insert(std::make_pair(p1, p2));
                break;
            }
        }
    }

    return collidingDiscs;
}

void World::handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs, const sf::Time& dt)
{
    //DeepSeek-generated 
    for (const auto& [p1, p2] : collidingDiscs)
    {
        auto& pos1 = p1->position_;
        auto& pos2 = p2->position_;
        auto& v1 = p1->velocity_;
        auto& v2 = p2->velocity_;
        const auto& m1 = p1->mass_;
        const auto& m2 = p2->mass_;
        const auto& r1 = p1->radius_;
        const auto& r2 = p2->radius_;

        // Normalenvektor der Kollision
        sf::Vector2f normal = pos2 - pos1;
        float distance = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        normal /= distance;

        // Tangentialvektor der Kollision
        sf::Vector2f tangent(-normal.y, normal.x);

        // Relative Geschwindigkeit
        sf::Vector2f relativeVelocity = v2 - v1;
        float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
        float velocityAlongTangent = relativeVelocity.x * tangent.x + relativeVelocity.y * tangent.y;

        // Wenn sich die Partikel voneinander entfernen, keine Kollision
        if (velocityAlongNormal > 0)
            continue;

        // Restitutionskoeffizient (Elastizität der Kollision)
        const float e = 1.f; // Vollständig elastisch

        // Impulsaustausch in der Normalenrichtung
        float jNormal = -(1 + e) * velocityAlongNormal;
        jNormal /= (1 / m1 + 1 / m2);

        // Impulsaustausch in der Tangentialrichtung (Reibung berücksichtigen)
        const float friction = 0.f; // Reibungskoeffizient
        float jTangent = -friction * velocityAlongTangent;
        jTangent /= (1 / m1 + 1 / m2);

        // Gesamtimpuls
        sf::Vector2f impulse = jNormal * normal + jTangent * tangent;

        // Anwenden des Impulses
        v1 -= impulse / m1;
        v2 += impulse / m2;

        // Positionen korrigieren, um Überlappungen zu vermeiden
        float overlap = (r1 + r2) - distance;
        pos1 -= overlap * normal / 2.0f;
        pos2 += overlap * normal / 2.0f;

        ++collisionCount_;
    }
}