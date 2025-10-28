#ifndef F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP
#define F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP

#include "core/FrameDTO.hpp"
#include "widgets/QSFMLWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

class SimulationConfigUpdater;

/**
 * @brief Widget displaying the simulation state by drawing a bunch of circles in different colors each frame based on
 * what is currently in the simulation
 */
class SimulationWidget : public QSFMLWidget
{
    Q_OBJECT
public:
    SimulationWidget(QWidget* parent);

    void setSimulationConfigUpdater(SimulationConfigUpdater* simulationConfigUpdater);

    void closeEvent(QCloseEvent* event) override;
    void toggleFullscreen();

signals:
    void requestExitFullscreen();

public slots:
    /**
     * @brief Clears the render window and displays circles based on the given `FrameDTO`
     */
    void render(const FrameDTO& frame, const cell::DiscTypeRegistry& discTypeRegistry,
                const std::map<std::string, sf::Color>& colorMap);

private:
    std::vector<sf::CircleShape> circles_;
    sf::Clock clock_;
    sf::CircleShape circularBounds_;
    SimulationConfigUpdater* simulationConfigUpdater_ = nullptr;
};

#endif /* F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP */
