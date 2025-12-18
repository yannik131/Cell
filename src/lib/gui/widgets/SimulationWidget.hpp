#ifndef F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP
#define F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP

#include "core/FrameDTO.hpp"
#include "widgets/QSFMLWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
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
    void render(const FrameDTO& frame, const cell::DiscTypeRegistry& discTypeRegistry);

private:
    void rebuildTypeShapes(const cell::DiscTypeRegistry& discTypeRegistry);

private:
    std::vector<sf::CircleShape> typeShapes_;
    sf::Clock clock_;
    SimulationConfigUpdater* simulationConfigUpdater_ = nullptr;
    int actualFPS_;
};

#endif /* F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP */
