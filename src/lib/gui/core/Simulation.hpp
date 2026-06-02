#ifndef C79C95D4_043A_4803_8C77_D97B81275A0C_HPP
#define C79C95D4_043A_4803_8C77_D97B81275A0C_HPP

#include "cell/SimulationConfig.hpp"
#include "cell/SimulationRecorder.hpp"
#include "cell/SimulationRunner.hpp"
#include "core/FrameDTO.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "core/Types.hpp"

#include <QObject>
#include <SFML/Graphics/CircleShape.hpp>

#include <vector>

using json = nlohmann::json;

/**
 * @brief Contains and runs the cell for the simulation
 */
class Simulation : public QObject
{
    Q_OBJECT
public:
    explicit Simulation(QObject* parent = nullptr);

    void start();
    void stop();
    bool isRunning() const;
    void reinitialize();
    void loadSettingsFromJson(const fs::path& settingsPath);
    void emitLastFrame();

    SimulationConfigUpdater& getSimulationConfigUpdater();
    const cell::SimulationConfig& getSimulationConfig() const;
    cell::SimulationContext getSimulationContext();

private:
    void initializeSimulationRecorder();

signals:
    void started();
    void stopped();
    void frame(const cell::SimulationRecorder::Frame& frame);
    void performanceData(const cell::SimulationRunner::PerformanceData& performanceData);
    void dataPoint(const cell::DataPoint& dataPoint);

private:
    cell::SimulationRunner simulationRunner_;
    std::unique_ptr<cell::SimulationRecorder> simulationRecorder_;
    SimulationConfigUpdater simulationConfigUpdater_;
};

#endif /* C79C95D4_043A_4803_8C77_D97B81275A0C_HPP */
