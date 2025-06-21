#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "Cell.hpp"
#include "FrameDTO.hpp"

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
     * @brief Resets the cell and emits the new cell setup as frame data
     */
    void reset();

    /**
     * @returns `true` if there are no discs in the simulation right now
     */
    bool worldIsEmpty() const;

    /**
     * @brief Populates a `FrameDTO` with the current cell state and emits it
     * @param noTimeElapsed If `true`, set the elapsed time information for the emitted `FrameDTO` to 0, indicating that
     * the DTO is just to be used for redrawing
     */
    void emitFrameData(bool noTimeElapsed = false);

signals:
    void frameData(const FrameDTO& data);

private:
    cell::Cell cell_;
};

#endif /* SIMULATION_HPP */
