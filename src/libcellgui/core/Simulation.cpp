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
    cell::GlobalSettings::get().lock();
    const auto& settings = cell::GlobalSettings::getSettings();

    sf::Clock clock;
    sf::Time timeSinceLastUpdate;

    while (true)
    {
        if (QThread::currentThread()->isInterruptionRequested())
            break;

        const sf::Time& dt = clock.restart();
        timeSinceLastUpdate += dt;

        while (timeSinceLastUpdate / static_cast<float>(settings.simulationTimeScale_) > settings.simulationTimeStep_)
        {
            timeSinceLastUpdate -= settings.simulationTimeStep_ / static_cast<float>(settings.simulationTimeScale_);

            cell_.update(settings.simulationTimeStep_);
            emitFrameData();
        }
    }

    cell::GlobalSettings::get().unlock();
}

void Simulation::reset()
{
    if (cell::GlobalSettings::getSettings().discTypeDistribution_.empty())
        return;

    cell_.reinitialize();
    emitFrameData();
}

bool Simulation::worldIsEmpty() const
{
    return cell_.getDiscs().empty();
}

void Simulation::emitFrameData(bool noTimeElapsed)
{
    FrameDTO frameDTO{.discs_ = cell_.getDiscs(),
                      .collisionCounts_ = cell_.getAndResetCollisionCounts(),
                      .elapsedSimulationTimeUs =
                          noTimeElapsed ? 0 : cell::GlobalSettings::getSettings().simulationTimeStep_.asMicroseconds()};

    emit frameData(frameDTO);
}
