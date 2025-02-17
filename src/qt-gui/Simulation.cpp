#include "Simulation.hpp"
#include "GlobalSettings.hpp"

#include <QThread>
#include <SFML/System/Clock.hpp>

#include <glog/logging.h>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
{
}

void Simulation::run()
{
    GlobalSettings::get().lock();
    const auto& settings = GlobalSettings::getSettings();

    sf::Clock clock;
    sf::Time timeSinceLastUpdate;
    sf::Time timeSinceLastFrame;
    sf::Time timeSinceLastCollisionUpdate;
    const sf::Time FrameTime = settings.guiFPS_ > 0 ? sf::milliseconds(1000 / settings.guiFPS_) : sf::seconds(1e6);

    while (true)
    {
        if (QThread::currentThread()->isInterruptionRequested())
        {
            // Reset collision count to 0 for the next run
            world_.getAndResetCollisionCount();
            break;
        }

        const sf::Time& dt = clock.restart();
        timeSinceLastUpdate += dt;
        timeSinceLastFrame += dt;

        if (timeSinceLastFrame > FrameTime)
        {
            emitFrameData();
            timeSinceLastFrame -= FrameTime;
        }

        if (timeSinceLastCollisionUpdate >= settings.collisionUpdateTime_)
        {
            int collisions = world_.getAndResetCollisionCount();
            float collisionsPerSecond = collisions / timeSinceLastCollisionUpdate.asSeconds();

            emit collisionData(static_cast<int>(std::round(collisionsPerSecond)));
            timeSinceLastCollisionUpdate = sf::Time::Zero;
        }

        while (timeSinceLastUpdate / settings.simulationTimeScale_ > settings.simulationTimeStep_)
        {
            timeSinceLastUpdate -= settings.simulationTimeStep_ / settings.simulationTimeScale_;
            world_.update(settings.simulationTimeStep_);

            timeSinceLastCollisionUpdate += settings.simulationTimeStep_;
        }
    }

    GlobalSettings::get().unlock();
}

void Simulation::reset()
{
    world_.reinitialize();
    emit sceneData(world_.discs());
    emitFrameData();
}

void Simulation::setWorldBounds(const sf::Vector2f& bounds)
{
    world_.setBounds(bounds);
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
