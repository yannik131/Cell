#ifndef SIMULATIONCANVAS_H
#define SIMULATIONCANVAS_H

#include "QSFMLCanvas.hpp"
#include "World.hpp"

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

class SimulationCanvas : public QSFMLCanvas
{
    Q_OBJECT
public:
    SimulationCanvas(QWidget* parent);

    int getAndResetCollisions();

private:
    void onInit();
    void onUpdate();

    void updateStatisticsText(const sf::Time& dt);
    
    void processEvents();
    void render();

private:
    sf::Clock clock_;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time timePerFrame_ = sf::milliseconds(30);
    const sf::Time TimePerFrameTarget = sf::milliseconds(2);
    
    World world_;
    sf::Font font_;
    sf::Text statisticsText_;
    
    sf::Time statisticsUpdateTime_ = sf::Time::Zero;
    int statisticsFrameCount_ = 0;
};

#endif /* SIMULATIONCANVAS_H */
