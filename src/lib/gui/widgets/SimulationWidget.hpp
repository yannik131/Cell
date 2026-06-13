#ifndef F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP
#define F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP

#include "core/FrameBuffer.hpp"
#include "core/Types.hpp"
#include "widgets/QSFMLWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Clock.hpp>

#include <QTimer>

class SimulationConfigUpdater;
class Simulation;
namespace cell
{
class Compartment;
}
using Frame = cell::SimulationRecorder::Frame;
using myClock = std::chrono::steady_clock;

/**
 * @brief Widget displaying the simulation state by drawing a bunch of circles in different colors each frame based on
 * what is currently in the simulation
 */
class SimulationWidget : public QSFMLWidget
{
    Q_OBJECT

public:
    SimulationWidget(QWidget* parent);

    void injectDependencies(SimulationConfigUpdater* simulationConfigUpdater, Simulation* simulation);
    void startRenderingTimer();
    void stopRenderingTimer();

    void closeEvent(QCloseEvent* event) override;
    void toggleFullscreen();
    void rebuildTypeShapes(const cell::DiscTypeRegistry& discTypeRegistry,
                           const cell::MembraneTypeRegistry& membraneTypeRegistry);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

signals:
    void requestExitFullscreen();
    void renderData(int targetFPS, int actualFPS, std::chrono::nanoseconds renderTime);

public slots:
    void queueFrameForRendering(Frame frame);
    void renderFrameImmediately(Frame Frame);
    void fitSimulationIntoView();

private:
    void drawFrame();
    double calculateIdealZoom() const;
    sf::Vector2i getWidgetSize() const;
    template <typename ObjectType, typename ObjectsGetter, typename NameSetter, typename ObjectsSetter>
    void addObjectAtCursor(const QPoint& cursorPosition, const std::string&, ObjectsGetter, NameSetter, ObjectsSetter);
    void addDiscAtCursor(const QPoint& cursorPosition, const std::string& typeName);
    void addMembraneAtCursor(const QPoint& cursorPosition, const std::string& typeName);

private:
    std::vector<sf::CircleShape> discTypeShapes_;
    std::vector<sf::CircleShape> membraneTypeShapes_;
    SimulationConfigUpdater* simulationConfigUpdater_ = nullptr;
    myClock::time_point currentRenderInterval_{};
    myClock::duration elapsedRenderTime_{};
    int renderedFrames_ = 0;
    FrameBuffer frameBuffer_;
    QTimer renderingTimer_;
};

#endif /* F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP */
