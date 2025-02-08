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
            startPositions_.push_back(sf::Vector2f(20 + i * 100, 20 + j * 50));
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

CollisionSet World::findCollidingParticles()
{
    NanoflannAdapter adapter(particles_);
    typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, NanoflannAdapter>, NanoflannAdapter,
                                                2>
        KDTree;
    KDTree kdtree(2, adapter);

    CollisionSet collidingParticles;
    int maxRadius = std::max_element(RadiusDistribution_.begin(), RadiusDistribution_.end(),
                                     [](const auto& a, const auto& b) { return a.second < b.second; })
                        ->second;

    for (auto& particle : particles_)
    {
        std::vector<nanoflann::ResultItem<uint32_t, float>> results;
        const float maxCollisionDistance = particle.getRadius() + maxRadius;
        const auto& position = particle.getPosition();

        kdtree.radiusSearch(&position.x, maxCollisionDistance * maxCollisionDistance, results);

        for (size_t i = 1; i < results.size(); ++i)
        {
            auto& otherParticle = particles_[results[i].first];
            const auto& otherPosition = otherParticle.getPosition();
            const double distance =
                std::pow(position.x - otherPosition.x, 2) + std::pow(position.y - otherPosition.y, 2);
            const double radiusSum = std::pow(particle.getRadius() + otherParticle.getRadius(), 2);

            if (distance <= radiusSum) {
                collidingParticles.insert(std::make_pair(&particle, &otherParticle));
                break;
            }
        }
    }

    return collidingParticles;
}

void World::handleParticleCollisions(const CollisionSet& collidingParticles, const sf::Time& dt)
{
    for (const auto& [p1, p2] : collidingParticles)
    {
        auto pos1 = p1->getPosition();
        auto pos2 = p2->getPosition();
        auto& v1 = p1->velocity;
        auto& v2 = p2->velocity;
        const auto& m1 = p1->mass;
        const auto& m2 = p2->mass;
        constexpr float PI = 3.14159265f;

        float phi;
        if (pos1.x == pos2.x)
            // Spezialfall pos1.x == pos2.x (die Kugeln befinden sich senkrecht übereinander)
            phi = pos2.y > pos1.y ? PI / 2 : -PI / 2;
        else
        {
            // Hier muss pos1.x <> pos2.x sein!
            phi = std::atan((pos2.y - pos1.y) / (pos2.x - pos1.x));
        }

        // Rechenterme, die in den Formeln mehrfach auftreten, werden erst einmal in Variablen gespeichert
        const float sinphi = std::sin(phi);
        const float cosphi = std::cos(phi);
        const float v1xsinphi = v1.x * sinphi;
        const float v1xcosphi = v1.x * cosphi;
        const float v1ysinphi = v1.y * sinphi;
        const float v1ycosphi = v1.y * cosphi;
        const float v2xsinphi = v2.x * sinphi;
        const float v2xcosphi = v2.x * cosphi;
        const float v2ysinphi = v2.y * sinphi;
        const float v2ycosphi = v2.y * cosphi;
        const float v1zaehler = (m1 - m2) * (v1xcosphi + v1ysinphi) + 2 * m2 * (v2xcosphi + v2ysinphi);
        const float v2zaehler = (m2 - m1) * (v2xcosphi + v2ysinphi) + 2 * m1 * (v1xcosphi + v1ysinphi);
        const float msum = m1 + m2;

        // Berechnung der neuen Geschwindigkeiten
        // (Die Bezeichnungen sind in Anlehnung an die Formeln gewählt, die in der ausführlichen
        // Herleitung - siehe PDF-Datei - gewählt wurden)
        v1.x = (v1xsinphi - v1ycosphi) * sinphi + v1zaehler * cosphi / msum;
        v1.y = (-v1xsinphi + v1ycosphi) * cosphi + v1zaehler * sinphi / msum;
        v2.x = (v2xsinphi - v2ycosphi) * sinphi + v2zaehler * cosphi / msum;
        v2.y = (-v2xsinphi + v2ycosphi) * cosphi + v2zaehler * sinphi / msum;

        // Wenn sich zwei Kugeln überlappen, müssen sie zuerst wieder getrennt werden, bevor es weiter geht.
        // Sonst kann es vorkommen, dass sie gar nicht mehr auseinander kommen, weil beim nächsten Schritt
        // wieder eine Überlappung (ein Stoß) registriert wird und die Richtung der Geschwindigkeiten
        // erneut geändert wird
        while ((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y) <= (p1->getRadius() + p2->getRadius()) * (p1->getRadius() + p2->getRadius()))
        {
            p1->move(v1 * dt.asSeconds());
            p2->move(v2 * dt.asSeconds());
            
            pos1 = p1->getPosition();
            pos2 = p2->getPosition();
        }
    }
}
