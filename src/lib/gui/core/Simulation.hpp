#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "cell/SimulationConfig.hpp"
#include "cell/SimulationContext.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "core/FrameDTO.hpp"

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

    void buildContext();

    /**
     * @brief Populates a `FrameDTO` with the current cell state and emits it
     * @param noTimeElapsed If `true`, set the elapsed time information for the emitted `FrameDTO` to 0, indicating that
     * the DTO is just to be used for redrawing
     */
    void emitFrameData(bool noTimeElapsed = false);

    const cell::SimulationConfig& getSimulationConfig() const override;
    void setSimulationConfig(const cell::SimulationConfig& simulationConfig) override;
    void setDiscTypes(const std::vector<cell::config::DiscType>& discTypes,
                      const std::unordered_set<std::string>& removedDiscTypes);

    const std::map<std::string, sf::Color>& getDiscTypeColorMap() const override;
    void setDiscTypeColorMap(const std::map<std::string, sf::Color>& discTypeColorMap) override;

    void registerConfigObserver(ConfigObserver observer) override;

signals:
    void frameData(const FrameDTO& data);

private:
    void notifyConfigObservers();

private:
    cell::SimulationConfig simulationConfig_;
    cell::SimulationContext simulationContext_;
    std::map<std::string, sf::Color> discTypeColorMap_;
    std::vector<ConfigObserver> observers_;
};

#endif /* SIMULATION_HPP */
