#ifndef ABSTRACTSIMULATIONBUILDER_HPP
#define ABSTRACTSIMULATIONBUILDER_HPP

#include "cell/SimulationConfig.hpp"

#include <SFML/Graphics/Color.hpp>

#include <functional>

using DiscTypeObserver = std::function<bool(const std::vector<cell::config::DiscType>&)>;

class AbstractSimulationBuilder
{
public:
    virtual cell::SimulationConfig& getSimulationConfig() = 0;

    virtual std::map<std::string, sf::Color>& getDiscTypeColorMap() = 0;
    virtual void registerDiscTypeObserver(DiscTypeObserver observer) = 0;
    virtual void notifyDiscTypeObservers() = 0;
};

#endif /* ABSTRACTSIMULATIONBUILDER_HPP */
