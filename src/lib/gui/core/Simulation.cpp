#include "Simulation.hpp"

#include <QThread>
#include <SFML/System/Clock.hpp>

#include <glog/logging.h>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
{
}

void Simulation::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate;

    while (true)
    {
        if (QThread::currentThread()->isInterruptionRequested())
            break;

        const sf::Time& dt = clock.restart();
        timeSinceLastUpdate += dt;

        // TODO
        break;
    }
}

void Simulation::reset()
{
    // TODO
    emitFrameData();
}

bool Simulation::worldIsEmpty() const
{
    // TODO
    return true;
}

void Simulation::emitFrameData(bool noTimeElapsed)
{
    // TODO
}
