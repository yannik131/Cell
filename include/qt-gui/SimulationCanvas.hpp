#ifndef SIMULATIONCANVAS_H
#define SIMULATIONCANVAS_H

#include "QSFMLCanvas.hpp"
#include "Simulation.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Clock.hpp>

class SimulationCanvas : public QSFMLCanvas
{
    Q_OBJECT
public:
    SimulationCanvas(QWidget* parent);

private:
    void onInit();
    void onUpdate();

private:
    sf::CircleShape shape_;
    Simulation simulation_;
    sf::Clock clock_;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
};

#endif /* SIMULATIONCANVAS_H */
