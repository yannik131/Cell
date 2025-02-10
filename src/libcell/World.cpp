#include "World.hpp"
#include "NanoflannAdapter.hpp"

#include <glog/logging.h>

#include <map>
#include <random>
#include <set>
#include <cmath>
#include <algorithm>

World::World(sf::RenderWindow& renderWindow)
    : renderWindow_(renderWindow)
{
    initializeStartPositions();
    buildScene();
    maxRadius_ = std::max_element(RadiusDistribution_.begin(), RadiusDistribution_.end(),
                                     [](const auto& a, const auto& b) { return a.second < b.second; })
                        ->second;
}

void World::update(const sf::Time& dt)
{
    for (auto& particle : particles_)
    {
        particle.update(dt);
        const auto& collidingParticles = findCollidingParticles();
        handleParticleCollisions(collidingParticles, dt);
        handleWorldBoundCollision(particle);
    }
}

void World::draw()
{
    for (const auto& particle : particles_)
        renderWindow_.draw(particle);
}

void World::buildScene()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution(0, 1);
    std::uniform_int_distribution<int> velocityDistribution(-350, 300);

    particles_.reserve(ParticleCount);
    std::map<int, int> counts;

    for (int i = 0; i < ParticleCount; ++i)
    {
        float randomNumber = distribution(gen);

        for (const auto& [probability, radius] : RadiusDistribution_)
        {
            if (randomNumber < probability)
            {
                counts[radius]++;
                Particle newParticle(radius);
                newParticle.setPosition(startPositions_.back());
                newParticle.velocity = sf::Vector2f(velocityDistribution(gen), velocityDistribution(gen));
                newParticle.setFillColor(Colors_.at(radius));

                particles_.push_back(newParticle);
                startPositions_.pop_back();
                
                break;
            }
        }
    }
    
    LOG(INFO) << "Radius distribution";
    for(const auto& [radius, count] : counts) {
        LOG(INFO) << radius << ": " << count << "/" << ParticleCount << " (" << count / static_cast<float>(ParticleCount) * 100 << "%)\n";
    }
}

void World::initializeStartPositions()
{
    startPositions_.reserve(625);
    std::random_device rd;
    std::mt19937 g(rd());

    for (int i = 0; i < 25; ++i)
    {
        for (int j = 0; j < 25; ++j)
            startPositions_.push_back(sf::Vector2f(20 + i * 20, 20 + j * 20));
    }

    std::shuffle(startPositions_.begin(), startPositions_.end(), g);
}

void World::handleWorldBoundCollision(Particle& particle)
{
    const float radius = particle.getRadius();
    const auto& position = particle.getPosition();
    const auto& worldBounds = renderWindow_.getSize();

    float dx, dy;
    
    if (position.x < radius)
    {
        dx = radius - position.x + 1;
        dy = dx * particle.velocity.y / particle.velocity.x;
        
        particle.move({dx, dy});
        particle.velocity.x = -particle.velocity.x;
    }
    else if (position.x > worldBounds.x - radius)
    {
        dx = -(position.x + radius - worldBounds.x + 1);
        dy = -(dx * particle.velocity.y / particle.velocity.x);
        
        particle.move({dx, dy});
        particle.velocity.x = -particle.velocity.x;
    }

    if (position.y < radius)
    {
        dy = radius - position.y + 1;
        dx = dy * particle.velocity.x / particle.velocity.y;
        
        particle.move({dx, dy});
        particle.velocity.y = -particle.velocity.y;
    }
    else if (position.y > worldBounds.y - radius)
    {
        dy = -(position.y + radius - worldBounds.y + 1);
        dx = -(dy * particle.velocity.x / particle.velocity.y);
        particle.move({dx, dy});
        particle.velocity.y = -particle.velocity.y;
    }
    
}

std::set<std::pair<Particle*, Particle*>> World::findCollidingParticles()
{
    NanoflannAdapter adapter(particles_);
    typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, NanoflannAdapter>, NanoflannAdapter,
                                                2>
        KDTree;
    KDTree kdtree(2, adapter);

    std::set<std::pair<Particle*, Particle*>> collidingParticles;

    for (auto& particle : particles_)
    {
        std::vector<nanoflann::ResultItem<uint32_t, float>> results;
        const float maxCollisionDistance = particle.getRadius() + maxRadius_;
        const auto& position = particle.getPosition();

        kdtree.radiusSearch(&position.x, maxCollisionDistance * maxCollisionDistance, results);

        for (size_t i = 1; i < results.size(); ++i)
        {
            auto& otherParticle = particles_[results[i].first];
            const float radiusSum = particle.getRadius() + otherParticle.getRadius();

            if (results[i].second <= radiusSum * radiusSum) {
                auto p1 = &particle;
                auto p2 = &otherParticle;
                if(p2 < p1) std::swap(p1, p2);
                collidingParticles.insert(std::make_pair(p1, p2));
                break;
            }
        }
    }

    return collidingParticles;
}

void World::handleParticleCollisions(const std::set<std::pair<Particle*, Particle*>>& collidingParticles, const sf::Time& dt)
{
    //DeepSeek-generated 
    for (const auto& [p1, p2] : collidingParticles)
    {
        auto pos1 = p1->getPosition();
        auto pos2 = p2->getPosition();
        auto& v1 = p1->velocity;
        auto& v2 = p2->velocity;
        const auto& m1 = p1->mass;
        const auto& m2 = p2->mass;
        const auto& r1 = p1->getRadius();
        const auto& r2 = p2->getRadius();

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
        const float friction = 0.01f; // Reibungskoeffizient
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

        p1->setPosition(pos1);
        p2->setPosition(pos2);
    }
}