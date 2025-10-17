#ifndef C79C95D4_043A_4803_8C77_D97B81275A0C_HPP
#define C79C95D4_043A_4803_8C77_D97B81275A0C_HPP

#include "cell/SimulationConfig.hpp"
#include "cell/SimulationFactory.hpp"
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
                      const std::map<std::string, sf::Color>& discTypeColorMap) override;

    const std::map<std::string, sf::Color>& getDiscTypeColorMap() const override;

    virtual void setMembraneTypes(const std::vector<cell::config::MembraneType>& membraneTypes,
                                  const std::unordered_set<std::string>& removedMembraneTypes,
                                  const std::map<std::string, sf::Color>& membraneTypeColorMap) = 0;

    virtual const std::map<std::string, sf::Color>& getMembraneTypeColorMap() const = 0;

    void registerConfigObserver(ConfigObserver observer) override;
    const cell::DiscTypeRegistry& getDiscTypeRegistry() override;

    bool cellIsBuilt() const;

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
    cell::SimulationFactory simulationFactory_;
    std::map<std::string, sf::Color> discTypeColorMap_;
    std::map<std::string, sf::Color> membraneTypeColorMap_;

    std::vector<ConfigObserver> configObservers_;
};

#endif /* C79C95D4_043A_4803_8C77_D97B81275A0C_HPP */
