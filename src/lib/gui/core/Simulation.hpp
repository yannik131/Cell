#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "cell/SimulationConfig.hpp"
#include "cell/SimulationContext.hpp"
#include "core/FrameDTO.hpp"

#include <QObject>
#include <SFML/System/Time.hpp>

#include <vector>

/**
 * @brief Contains and runs the cell for the simulation
 */
class Simulation : public QObject
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

public:
    void receiveDiscTypes(const std::vector<cell::config::DiscType>& discTypes,
                          const std::map<std::string, sf::Color>& discTypeColorMap);
    void receiveReactions(const std::vector<cell::config::Reaction>& reactions);
    void receiveSetup(const cell::config::Setup& setup);

    void emitDiscTypes();

signals:
    void frameData(const FrameDTO& data);
    void discTypes(const std::vector<cell::config::DiscType>& discTypes,
                   const std::map<std::string, sf::Color>& discTypeColorMap);

private:
    void tryBuildContext(bool throwIfIncomplete = false);

private:
    cell::SimulationConfig simulationConfig_;
    cell::SimulationContext simulationContext;
    std::map<std::string, sf::Color> discTypeColorMap_;
    bool setupReceived_ = false;
};

#endif /* SIMULATION_HPP */
