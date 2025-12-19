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
    void renderData(int targetFPS, int actualFPS, std::chrono::nanoseconds renderTime);

public slots:
    void render(const FrameDTO& frame, const cell::DiscTypeRegistry& discTypeRegistry);

private:
    void rebuildTypeShapes(const cell::DiscTypeRegistry& discTypeRegistry);
    void drawFrame(const FrameDTO& frame, const cell::DiscTypeRegistry& discTypeRegistry);
    void restartTimers(const FrameDTO& frame);

private:
    std::vector<sf::CircleShape> typeShapes_;
    SimulationConfigUpdater* simulationConfigUpdater_ = nullptr;
    std::chrono::steady_clock::time_point currentRenderInterval_;
    std::chrono::steady_clock::time_point nextAllowedRenderTime_;
    std::chrono::steady_clock::duration elapsedRenderTime_;
    int renderedFrames_;
    bool renderingStarted_ = false;
};

#endif /* F8B0BFE1_0E51_424A_A3DE_69E0B57425D7_HPP */
