#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "cell/SimulationConfig.hpp"
#include "cell/SimulationContext.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "core/FrameDTO.hpp"
#include "core/Types.hpp"

#include <QObject>
#include <SFML/System/Time.hpp>

#include <vector>

/**
 * @brief Contains and runs the cell for the simulation
 */
class Simulation : public QObject, public AbstractSimulationBuilder
{
    Q_OBJECT
public:
    explicit Simulation(QObject* parent = nullptr);

    /**
     * @brief Runs the simulation and emits information about the current frame (disc positions etc.). Stops the
     * simulation if an interruption of the current thread was requested
     */
    void run();

    void buildContext(const cell::SimulationConfig& = {});
    void rebuildContext();

    const cell::SimulationConfig& getSimulationConfig() const override;
    void setSimulationConfig(const cell::SimulationConfig& simulationConfig) override;
    void setDiscTypes(const std::vector<cell::config::DiscType>& discTypes,
                      const std::unordered_set<std::string>& removedDiscTypes,
                      const std::map<std::string, sf::Color>& discTypeColorMap);

    const std::map<std::string, sf::Color>& getDiscTypeColorMap() const override;

    void registerConfigObserver(ConfigObserver observer) override;

    cell::DiscTypeResolver getDiscTypeResolver() const;
    bool contextIsBuilt() const;

    /**
     * @todo Resets the internal config to a default constructed object and sends signals to all observers so that they
     * display that config
     */
    void loadDefaultConfig();

    void saveConfigToFile(const fs::path& path) const;
    void loadConfigFromFile(const fs::path& path);

    void emitFrame(RedrawOnly redrawOnly);

signals:
    void frame(const FrameDTO& frame);

private:
    void notifyConfigObservers();

private:
    cell::SimulationConfig simulationConfig_;
    cell::SimulationContext simulationContext_;
    std::map<std::string, sf::Color> discTypeColorMap_;

    std::vector<ConfigObserver> configObservers_;
};

#endif /* SIMULATION_HPP */
