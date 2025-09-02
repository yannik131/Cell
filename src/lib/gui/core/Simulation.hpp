#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "FrameDTO.hpp"
#include "cell/SimulationConfig.hpp"

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

    /**
     * @brief Populates a `FrameDTO` with the current cell state and emits it
     * @param noTimeElapsed If `true`, set the elapsed time information for the emitted `FrameDTO` to 0, indicating that
     * the DTO is just to be used for redrawing
     */
    void emitFrameData(bool noTimeElapsed = false);

public slots:
    void receiveDiscTypes(const std::vector<cell::config::DiscType>& discTypes);
    void receiveReactions(const std::vector<cell::config::Reaction>& reactions);
    void receiveState(const cell::config::StateSetup& stateSetup);

signals:
    void frameData(const FrameDTO& data);
};

#endif /* SIMULATION_HPP */
