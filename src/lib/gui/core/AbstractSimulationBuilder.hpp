#ifndef ABSTRACTSIMULATIONBUILDER_HPP
#define ABSTRACTSIMULATIONBUILDER_HPP

#include "cell/SimulationConfig.hpp"
#include "cell/Types.hpp"

#include <SFML/Graphics/Color.hpp>

#include <functional>
#include <string>
#include <unordered_set>

namespace cell
{
class Disc;
}

using ConfigObserver =
    std::function<void(const cell::SimulationConfig& config, const std::map<std::string, sf::Color>& discTypeColorMap)>;

class AbstractSimulationBuilder
{
public:
    virtual const cell::SimulationConfig& getSimulationConfig() const = 0;
    virtual void setSimulationConfig(const cell::SimulationConfig& simulationConfig) = 0;
    virtual void setDiscTypes(const std::vector<cell::config::DiscType>& discTypes,
                              const std::unordered_set<std::string>& removedDiscTypes,
                              const std::map<std::string, sf::Color>& discTypeColorMap) = 0;

    virtual const std::map<std::string, sf::Color>& getDiscTypeColorMap() const = 0;

    virtual void registerConfigObserver(ConfigObserver observer) = 0;
    virtual cell::DiscTypeResolver getDiscTypeResolver() const = 0;
};

#endif /* ABSTRACTSIMULATIONBUILDER_HPP */
