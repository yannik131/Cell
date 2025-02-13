#ifndef SIMULATIONSETTINGS_HPP
#define SIMULATIONSETTINGS_HPP

#include <SFML/System/Time.hpp>

#include <QtCore/QMetaType>

struct SimulationSettings {
    sf::Time simulationTimeStep = sf::milliseconds(5);
    sf::Time frameTime = sf::milliseconds(1000 / 40);
    sf::Time collisionUpdateTime = sf::seconds(1);
    int numberOfDiscs = 50;
};

Q_DECLARE_METATYPE(SimulationSettings);

#endif /* SIMULATIONSETTINGS_HPP */
