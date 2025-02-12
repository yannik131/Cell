#include "Simulation.hpp"

#include <SFML/System/Clock.hpp>

#include <glog/logging.h>

void Simulation::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate;
    sf::Time timeSinceLastFrame;
    sf::Time timeSinceLastCollisionUpdate;

    emit sceneData(world_.discs());
    
    while(true) {
        const sf::Time& dt = clock.restart();
        timeSinceLastUpdate += dt;
        timeSinceLastFrame += dt;

        if(timeSinceLastFrame > FrameTime) {
            emitFrameData();
            timeSinceLastFrame = sf::Time::Zero;
        }

        while(timeSinceLastUpdate > SimulationTimeStep) {
            timeSinceLastUpdate -= SimulationTimeStep;
            
            world_.update(SimulationTimeStep);

            timeSinceLastCollisionUpdate += SimulationTimeStep;
            if(timeSinceLastCollisionUpdate >= CollisionUpdateTime)
            {
                int collisions = world_.getAndResetCollisionCount();
                emit collisionData(collisions);
                timeSinceLastCollisionUpdate = sf::Time::Zero;
            }
        }
    }
}

void Simulation::emitFrameData()
{
    const auto& discs = world_.discs();
    FrameDTO frameDTO;
    frameDTO.discs_.reserve(discs.size());

    for(int i = 0; i < discs.size(); ++i)
        frameDTO.discs_.push_back(GUIDisc(i, discs[i].position_));

    emit frameData(frameDTO);
}

Simulation::Simulation(QObject* parent) : QObject(parent)
{
}
