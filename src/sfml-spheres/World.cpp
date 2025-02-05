#include "World.hpp"
#include "NanoflannAdapter.hpp"

#include <random>

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
        handleWorldBoundCollision(particle);
        handleParticleCollisions();
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
    std::uniform_int_distribution<int> velocityDistribution(-100, 100);

    particles_.reserve(ParticleCount);

    for (int i = 0; i < ParticleCount; ++i)
    {
        float randomNumber = distribution(gen);

        for (const auto& [probability, radius] : RadiusDistribution_)
        {
            if (randomNumber < probability)
            {
                Particle newParticle(radius);
                newParticle.setOrigin(sf::Vector2f(radius / 2.f, radius / 2.f));
                newParticle.setPosition(startPositions_.back());
                newParticle.velocity = sf::Vector2f(velocityDistribution(gen), velocityDistribution(gen));
                newParticle.setFillColor(Colors_.at(radius));

                particles_.push_back(newParticle);
                startPositions_.pop_back();
            }
        }
    }
}

void World::initializeStartPositions()
{
    startPositions_.reserve(625);

    for (int i = 0; i < 25; ++i)
    {
        for (int j = 0; j < 25; ++j)
            startPositions_.push_back(sf::Vector2f(20 + i * 40, 20 + j * 40));
    }

    std::random_shuffle(startPositions_.begin(), startPositions_.end());
}

void World::handleWorldBoundCollision(Particle& particle)
{
    const float radius = particle.getRadius();
    const auto& position = particle.getPosition();
    const auto& worldBounds = renderWindow_.getSize();

    if (position.x < 0)
    {
        particle.velocity.x = -particle.velocity.x;
        particle.setPosition(0, position.y);
    }
    else if (position.x > worldBounds.x - 2 * radius)
    {
        particle.velocity.x = -particle.velocity.x;
        particle.setPosition(worldBounds.x - 2 * radius, position.y);
    }

    if (position.y < 0)
    {
        particle.velocity.y = -particle.velocity.y;
        particle.setPosition(position.x, 0);
    }
    else if (position.y > worldBounds.y - 2 * radius)
    {
        particle.velocity.y = -particle.velocity.y;
        particle.setPosition(position.x, worldBounds.y - 2 * radius);
    }
}

void World::handleParticleCollisions()
{
    NanoflannAdapter adapter(particles_);
    typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, NanoflannAdapter>, NanoflannAdapter,
                                                2>
        KDTree;
    KDTree kdtree(2, adapter);
    kdtree.buildIndex();
    
    std::map<Particle, bool> collisions;
    
    for(auto& particle : particles_) {
        if(collisions[particle])
            continue;
        std::vector<nanoflann::ResultItem<uint32_t, float>> results;
        const float maximumPossibleCollisionDistance = particle.getRadius() + 15;
        const sf::Vector2f position = particle.getPosition();
        kdtree.radiusSearch(&position.x, std::pow(maximumPossibleCollisionDistance, 2), results);
        
        for(int i = 1; i < results.size(); ++i) {
            auto& otherParticle = particles_[results[i].first];
            const auto& position = particle.getPosition();
            const auto& otherPosition = otherParticle.getPosition();
            
            //Check for collision
            if(std::pow(position.x - otherPosition.x, 2) + std::pow(position.y - otherPosition.y, 2) > particle.getRadius() + otherParticle.getRadius())
                continue;
                
            particle.velocity = -particle.velocity;
            otherParticle.velocity = -otherParticle.velocity;
            collisions[otherParticle] = true;
            break;
        }
    }
}
