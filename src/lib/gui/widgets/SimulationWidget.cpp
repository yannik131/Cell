#include "widgets/SimulationWidget.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "core/Utility.hpp"

#include "SimulationWidget.hpp"
#include <QApplication>
#include <QCloseEvent>
#include <QLayout>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>

namespace chrono = std::chrono;

SimulationWidget::SimulationWidget(QWidget* parent)
    : QSFMLWidget(parent)
{
}

void SimulationWidget::setSimulationConfigUpdater(SimulationConfigUpdater* simulationConfigUpdater)
{
    simulationConfigUpdater_ = simulationConfigUpdater;
}

void SimulationWidget::closeEvent(QCloseEvent* event)
{
    // If the widget is full screen, exit full screen instead of destroying it
    emit requestExitFullscreen();
    event->ignore();
}

void SimulationWidget::toggleFullscreen()
{
    static QWidget* origParent = parentWidget();
    static QWidget* placeholder = nullptr;

    if (!isFullScreen())
    {
        placeholder = new QWidget(origParent);
        origParent->layout()->replaceWidget(this, placeholder);
        setParent(nullptr);
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        showFullScreen();
    }
    else
    {
        if (!placeholder) // clang-tidy bullshit
            throw ExceptionWithLocation("The simulation widget was initialized in full screen mode for some reason.");

        setParent(origParent);
        origParent->layout()->replaceWidget(placeholder, this);
        placeholder->deleteLater();
        placeholder = nullptr;
        showNormal();
    }

    const auto widgetSize = getWidgetSize();
    sf::RenderWindow::close();
    sf::RenderWindow::create((sf::WindowHandle)winId());
    sf::RenderWindow::setSize(static_cast<sf::Vector2u>(widgetSize));
    QSFMLWidget::resetView(Zoom{calculateIdealZoom()}, static_cast<sf::Vector2f>(widgetSize));
}

void SimulationWidget::fitSimulationIntoView()
{
    const auto zoom = calculateIdealZoom();
    resetView(Zoom{zoom});
}

void SimulationWidget::rebuildTypeShapes(const cell::DiscTypeRegistry& discTypeRegistry)
{
    typeShapes_.resize(discTypeRegistry.getValues().size());

    for (const auto& type : discTypeRegistry.getValues())
    {
        const auto ID = discTypeRegistry.getIDFor(type.getName());

        sf::CircleShape circleShape;
        circleShape.setRadius(static_cast<float>(type.getRadius()));
        circleShape.setFillColor(simulationConfigUpdater_->getDiscTypeColorMap().at(type.getName()));
        circleShape.setOrigin(sf::Vector2f{static_cast<float>(type.getRadius()), static_cast<float>(type.getRadius())});

        typeShapes_[ID] = circleShape;
    }
}

void SimulationWidget::render(const FrameDTO& frame, const cell::DiscTypeRegistry& discTypeRegistry)
{
    using clock = std::chrono::steady_clock;
    using namespace std::chrono;

    restartTimers(frame);
    const auto targetRenderTime =
        duration_cast<clock::duration>(duration<double>(1.0 / simulationConfigUpdater_->getFPS()));

    if (clock::now() < nextAllowedRenderTime_)
        return;

    const auto start = clock::now();
    drawFrame(frame, discTypeRegistry);
    const auto now = clock::now();
    const auto renderTime = now - start;

    if (frame.elapsedSimulationTimeUs == 0)
        return;

    nextAllowedRenderTime_ = now + targetRenderTime;

    ++renderedFrames_;
    elapsedRenderTime_ += renderTime;
    if (now - currentRenderInterval_ >= 1s)
    {
        emit renderData(simulationConfigUpdater_->getFPS(), renderedFrames_, elapsedRenderTime_ / renderedFrames_);
        renderedFrames_ = 0;
        currentRenderInterval_ = now;
        elapsedRenderTime_ = 0ns;
    }
}

void SimulationWidget::drawFrame(const FrameDTO& frame, const cell::DiscTypeRegistry& discTypeRegistry)
{
    sf::RenderWindow::clear(sf::Color::Black);
    rebuildTypeShapes(discTypeRegistry); // TODO Only update cache when needed, same for compartments/membranes

    for (const auto& disc : frame.discs_)
    {
        typeShapes_[disc.getTypeID()].setPosition(utility::toVector2f(disc.getPosition()));
        sf::RenderWindow::draw(typeShapes_[disc.getTypeID()]);
    }

    for (const auto& membrane : frame.membranes_)
    {
        auto copy = membrane;
        copy.setOutlineThickness(static_cast<float>(QSFMLWidget::getCurrentZoom()));
        sf::RenderWindow::draw(copy);
    }

    sf::RenderWindow::display();
}

void SimulationWidget::restartTimers(const FrameDTO& frame)
{
    using clock = std::chrono::steady_clock;
    using namespace std::chrono;

    // Simulation in progress, do nothing
    if (renderingStarted_ && frame.elapsedSimulationTimeUs > 0)
        return;

    // Simulation stopped
    if (frame.elapsedSimulationTimeUs == 0)
    {
        renderingStarted_ = false;
        return;
    }

    // Simulation was stopped and now started again - reset timers
    if (!renderingStarted_ && frame.elapsedSimulationTimeUs > 0)
    {
        renderedFrames_ = 0;
        currentRenderInterval_ = clock::now();
        nextAllowedRenderTime_ = clock::now();
        elapsedRenderTime_ = 0ns;
        renderingStarted_ = true;
    }
}

double SimulationWidget::calculateIdealZoom() const
{
    if (!simulationConfigUpdater_)
        return 1.0;

    const auto config = simulationConfigUpdater_->getSimulationConfig();
    const auto widgetSize = getWidgetSize();
    const auto smallestEdge = std::min(widgetSize.x / 2, widgetSize.y / 2);

    return config.cellMembraneType.radius / smallestEdge;
}

sf::Vector2i SimulationWidget::getWidgetSize() const
{
    if (isFullScreen())
        return static_cast<sf::Vector2i>(sf::VideoMode::getDesktopMode().size);

    return sf::Vector2i(QWidget::size().width(), QWidget::size().height());
}
