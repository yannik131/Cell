#include "Simulation.hpp"
#include "GlobalSettings.hpp"

#include <QThread>
#include <SFML/System/Clock.hpp>

#include <glog/logging.h>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
    , worldDiscs_(world_.discs())
{
}

void Simulation::run()
{
    GlobalSettings::get().lock();
    const auto& settings = GlobalSettings::getSettings();

    sf::Clock clock;
    sf::Time timeSinceLastUpdate;

    while (true)
    {
        if (QThread::currentThread()->isInterruptionRequested())
        {
            world_.getAndResetCollisionCount(); // Reset collision count to 0 for the next run
            break;
        }

        const sf::Time& dt = clock.restart();
        timeSinceLastUpdate += dt;

        while (timeSinceLastUpdate / settings.simulationTimeScale_ > settings.simulationTimeStep_)
        {
            timeSinceLastUpdate -= settings.simulationTimeStep_ / settings.simulationTimeScale_;

            world_.update(settings.simulationTimeStep_);
            emitFrameData();
        }
    }

    GlobalSettings::get().unlock();
}

void Simulation::reset()
{
    world_.reinitialize();
    emit sceneData(worldDiscs_);
    emitFrameData();
}

void Simulation::setWorldBounds(const sf::Vector2f& bounds)
{
    world_.setBounds(bounds);
}

void Simulation::emitFrameData()
{
    // Benchmarks have shown that a single emit of 1B takes about as long as one of 40kB (5-10us, several 100x slower
    // than calculating a simulation step)
    FrameDTO frameDTO{.discs_ = worldDiscs_,
                      .collisionCounts_ = world_.getAndResetCollisionCount(),
                      .simulationTimeStepUs = GlobalSettings::getSettings().simulationTimeStep_.asMicroseconds()};

    emit frameData(frameDTO);
}