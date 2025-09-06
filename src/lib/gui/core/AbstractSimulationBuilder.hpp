#ifndef ABSTRACTSIMULATIONBUILDER_HPP
#define ABSTRACTSIMULATIONBUILDER_HPP

#include "cell/SimulationConfig.hpp"

#include <SFML/Graphics/Color.hpp>

#include <functional>

using DiscTypeObserver = std::function<bool(const std::vector<cell::config::DiscType>&)>;

class AbstractSimulationBuilder
{
public:
    virtual const cell::SimulationConfig& getSimulationConfig() const = 0;
    virtual void setSimulationConfig(const cell::SimulationConfig& simulationConfig) = 0;

    virtual const std::map<std::string, sf::Color>& getDiscTypeColorMap() const = 0;
    virtual void setDiscTypeColorMap(const std::map<std::string, sf::Color>& discTypeColorMap) = 0;

    virtual void registerDiscTypeObserver(DiscTypeObserver observer) = 0;
};

#endif /* ABSTRACTSIMULATIONBUILDER_HPP */
