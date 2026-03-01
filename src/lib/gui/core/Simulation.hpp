#ifndef C79C95D4_043A_4803_8C77_D97B81275A0C_HPP
#define C79C95D4_043A_4803_8C77_D97B81275A0C_HPP

#include "cell/SimulationConfig.hpp"
#include "cell/SimulationFactory.hpp"
#include "core/FrameDTO.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "core/Types.hpp"

#include <QObject>
#include <SFML/Graphics/CircleShape.hpp>

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

    void buildContext(const cell::SimulationConfig& = {});
    void rebuildContext();

    const cell::DiscTypeRegistry& getDiscTypeRegistry();

    SimulationConfigUpdater& getSimulationConfigUpdater();

    bool cellIsBuilt() const;

    void emitFrame(RedrawOnly redrawOnly);

private:
    sf::CircleShape circleShapeFromCompartment(const cell::Compartment& compartment);

signals:
    void frame(const FrameDTO& frame);
    void simulationData(double targetScale, double actualScale, std::chrono::nanoseconds updateTime,
                        std::chrono::nanoseconds simulationUpdateTime);

private:
    cell::SimulationFactory simulationFactory_;
    SimulationConfigUpdater simulationConfigUpdater_;
    std::vector<sf::CircleShape> membranes_;
};

#endif /* C79C95D4_043A_4803_8C77_D97B81275A0C_HPP */
