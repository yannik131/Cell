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
        if (QThread::currentThread()->isInterruptionRequested()) {
            //Reset collision count to 0 for the next run
            world_.getAndResetCollisionCount();
            return;
        }
        
        const sf::Time& dt = clock.restart();
        timeSinceLastUpdate += dt;
        timeSinceLastFrame += dt;

        if (timeSinceLastFrame > simulationSettings_.frameTime)
        {
            emitFrameData();
            timeSinceLastFrame = sf::Time::Zero;
        }

        while (timeSinceLastUpdate > simulationSettings_.simulationTimeStep)
        {
            timeSinceLastUpdate -= simulationSettings_.simulationTimeStep;

            world_.update(simulationSettings_.simulationTimeStep);

            timeSinceLastCollisionUpdate += simulationSettings_.simulationTimeStep;
            if (timeSinceLastCollisionUpdate >= simulationSettings_.collisionUpdateTime)
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

void Simulation::setWorldBounds(const sf::Vector2f& bounds)
{
    world_.setBounds(bounds);
}

void Simulation::setSimulationSettings(const SimulationSettings& simulationSettings)
{
    bool discCountChanged = simulationSettings_.numberOfDiscs != simulationSettings.numberOfDiscs;
    simulationSettings_ = simulationSettings;
    if(discCountChanged) {
        world_.setNumberOfDiscs(simulationSettings_.numberOfDiscs);
        reset();
    }
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
