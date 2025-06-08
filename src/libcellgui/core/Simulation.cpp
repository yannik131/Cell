#include "Simulation.hpp"
#include "GlobalSettings.hpp"

#include <QThread>
#include <SFML/System/Clock.hpp>

#include <glog/logging.h>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
    , worldDiscs_(cell_.discs())
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
            break;

        const sf::Time& dt = clock.restart();
        timeSinceLastUpdate += dt;

        while (timeSinceLastUpdate / settings.simulationTimeScale_ > settings.simulationTimeStep_)
        {
            timeSinceLastUpdate -= settings.simulationTimeStep_ / settings.simulationTimeScale_;

            cell_.update(settings.simulationTimeStep_);
            emitFrameData();
        }
    }

    GlobalSettings::get().unlock();
}

void Simulation::reset()
{
    cell_.reinitialize();
    emitFrameData();
}

void Simulation::setWorldBounds(const sf::Vector2f& bounds)
{
    cell_.setBounds(bounds);
}

void Simulation::emitFrameData()
{
    FrameDTO frameDTO{.discs_ = worldDiscs_,
                      .collisionCounts_ = cell_.getAndResetCollisionCount(),
                      .simulationTimeStepUs = GlobalSettings::getSettings().simulationTimeStep_.asMicroseconds()};

    emit frameData(frameDTO);
}