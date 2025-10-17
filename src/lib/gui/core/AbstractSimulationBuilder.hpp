#ifndef B6033B12_555B_4FC6_861D_78AD534B193C_HPP
#define B6033B12_555B_4FC6_861D_78AD534B193C_HPP

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

// TODO apply Interface Segregation Principle

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

    virtual void setMembraneTypes(const std::vector<cell::config::MembraneType>& membraneTypes,
                                  const std::unordered_set<std::string>& removedMembraneTypes,
                                  const std::map<std::string, sf::Color>& membraneTypeColorMap) = 0;

    virtual const std::map<std::string, sf::Color>& getMembraneTypeColorMap() const = 0;

    virtual void registerConfigObserver(ConfigObserver observer) = 0;
    virtual const cell::DiscTypeRegistry& getDiscTypeRegistry() = 0;

    virtual ~AbstractSimulationBuilder() = default;
};

#endif /* B6033B12_555B_4FC6_861D_78AD534B193C_HPP */
