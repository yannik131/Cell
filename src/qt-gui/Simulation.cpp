#include "Simulation.hpp"

#include <QThread>
#include <SFML/System/Clock.hpp>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
{
}

void Simulation::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate;
    sf::Time timeSinceLastFrame;
    sf::Time timeSinceLastCollisionUpdate;

    while (true)
    {
        if (QThread::currentThread()->isInterruptionRequested())
            return;
        
        const sf::Time& dt = clock.restart();
        timeSinceLastUpdate += dt;
        timeSinceLastFrame += dt;

        if (timeSinceLastFrame > FrameTime)
        {
            emitFrameData();
            timeSinceLastFrame = sf::Time::Zero;
        }

        while (timeSinceLastUpdate > SimulationTimeStep)
        {
            timeSinceLastUpdate -= SimulationTimeStep;

            world_.update(SimulationTimeStep);

            timeSinceLastCollisionUpdate += SimulationTimeStep;
            if (timeSinceLastCollisionUpdate >= CollisionUpdateTime)
            {
                int collisions = world_.getAndResetCollisionCount();
                emit collisionData(collisions);
                timeSinceLastCollisionUpdate = sf::Time::Zero;
            }
        }
    }
}

void Simulation::reset()
{
    world_.reset();
    emit sceneData(world_.discs());
    emitFrameData();
}

void Simulation::emitFrameData()
{
    const auto& discs = world_.discs();
    FrameDTO frameDTO;
    frameDTO.discs_.reserve(discs.size());

    for (int i = 0; i < discs.size(); ++i)
        frameDTO.discs_.push_back(GUIDisc(i, discs[i].position_));

    emit frameData(frameDTO);
}
