#ifndef F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP
#define F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP

#include "core/FrameDTO.hpp"
#include "widgets/QSFMLWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Clock.hpp>

class SimulationConfigUpdater;
class Simulation;
namespace cell
{
class Compartment;
}

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

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

signals:
    void requestExitFullscreen();
    void renderData(int targetFPS, int actualFPS, std::chrono::nanoseconds renderTime);

public slots:
    void render(const FrameDTO& frame, const cell::DiscTypeRegistry& discTypeRegistry);
    void fitSimulationIntoView();

private:
    void rebuildTypeShapes(const cell::DiscTypeRegistry& discTypeRegistry);
    void drawFrame(const FrameDTO& frame, const cell::DiscTypeRegistry& discTypeRegistry);
    double calculateIdealZoom() const;
    sf::Vector2i getWidgetSize() const;
    template <typename ObjectType, typename ObjectsGetter, typename NameSetter, typename ObjectsSetter>
    void addObjectAtCursor(const QPoint& cursorPosition, const std::string&, ObjectsGetter, NameSetter, ObjectsSetter);
    void addDiscAtCursor(const QPoint& cursorPosition, const std::string& typeName);
    void addMembraneAtCursor(const QPoint& cursorPosition, const std::string& typeName);
    sf::CircleShape circleShapeFromCompartment(const cell::Compartment& compartment);

private:
    std::vector<sf::CircleShape> typeShapes_;
    SimulationConfigUpdater* simulationConfigUpdater_ = nullptr;
    std::chrono::steady_clock::time_point currentRenderInterval_{};
    std::chrono::steady_clock::time_point nextAllowedRenderTime_{};
    std::chrono::steady_clock::duration elapsedRenderTime_{};
    int renderedFrames_ = 0;
    const Simulation* simulation_; // TODO use this for registry access
};

#endif /* F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP */
