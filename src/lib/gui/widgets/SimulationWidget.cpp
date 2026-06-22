#include "widgets/SimulationWidget.hpp"
#include "core/Simulation.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "core/Utility.hpp"

#include <QApplication>
#include <QCloseEvent>
#include <QContextMenuEvent>
#include <QLayout>
#include <QMenu>
#include <QMessageBox>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>

namespace chrono = std::chrono;

SimulationWidget::SimulationWidget(QWidget* parent)
    : QSFMLWidget(parent)
{
    setContextMenuPolicy(Qt::DefaultContextMenu);
    connect(&renderingTimer_, &QTimer::timeout, this, &SimulationWidget::drawFrame);
    renderingTimer_.setTimerType(Qt::PreciseTimer);
}

void SimulationWidget::injectDependencies(SimulationConfigUpdater* simulationConfigUpdater, Simulation* simulation)
{
    simulationConfigUpdater_ = simulationConfigUpdater;
    renderingTimer_.setInterval(qRound(1000.0 / simulationConfigUpdater->getFPS()));

    connect(simulationConfigUpdater, &SimulationConfigUpdater::fpsChanged,
            [&](int FPS) { renderingTimer_.setInterval(qRound(1000.0 / FPS)); });
    connect(simulation, &Simulation::simulationContextChanged, this, [&](cell::SimulationContext simulationContext)
            { rebuildTypeShapes(simulationContext.discTypeRegistry, simulationContext.membraneTypeRegistry); });
}

void SimulationWidget::startRenderingTimer()
{
    renderingTimer_.start();
    currentRenderInterval_ = myClock::now();
    renderedFrames_ = 0;
    disableRenderSignal();
}

void SimulationWidget::stopRenderingTimer()
{
    renderingTimer_.stop();
    enableRenderSignal();
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

void SimulationWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if (renderingTimer_.isActive())
        return;

    QMenu menu(this);
    const QPoint cursorPosition = event->pos();

    auto fillMenu = [this, cursorPosition](QMenu* subMenu, const auto& types, auto handler)
    {
        if (types.empty())
        {
            QAction* action = subMenu->addAction("No types defined");
            action->setEnabled(false);
            return;
        }

        for (const auto& type : types)
        {
            QAction* action = subMenu->addAction(QString::fromStdString(type.name));
            const auto typeName = type.name;
            connect(action, &QAction::triggered, this,
                    [this, handler, cursorPosition, typeName]() { (this->*handler)(cursorPosition, typeName); });
        }
    };

    const auto& config = simulationConfigUpdater_->getSimulationConfig();
    fillMenu(menu.addMenu("Add membrane..."), config.membraneTypes, &SimulationWidget::addMembraneAtCursor);
    fillMenu(menu.addMenu("Add disc..."), config.discTypes, &SimulationWidget::addDiscAtCursor);

    menu.exec(event->globalPos());
}

void SimulationWidget::fitSimulationIntoView()
{
    const auto zoom = calculateIdealZoom();
    resetView(Zoom{zoom});
}

void SimulationWidget::rebuildTypeShapes(const cell::DiscTypeRegistry& discTypeRegistry,
                                         const cell::MembraneTypeRegistry& membraneTypeRegistry)
{
    discTypeShapes_.resize(discTypeRegistry.getValues().size());
    membraneTypeShapes_.resize(membraneTypeRegistry.getValues().size());
    sf::CircleShape circleShape;

    for (const auto& type : discTypeRegistry.getValues())
    {
        const auto ID = discTypeRegistry.getIDFor(type.getName());

        circleShape.setRadius(static_cast<float>(type.getRadius()));
        circleShape.setFillColor(simulationConfigUpdater_->getDiscTypeColorMap().at(type.getName()));
        circleShape.setOrigin(sf::Vector2f{static_cast<float>(type.getRadius()), static_cast<float>(type.getRadius())});

        discTypeShapes_[ID] = circleShape;
    }

    circleShape.setPointCount(100);
    for (const auto& type : membraneTypeRegistry.getValues())
    {
        const auto ID = membraneTypeRegistry.getIDFor(type.getName());
        const auto R = static_cast<float>(type.getRadius());

        if (type.getName() == cell::config::cellMembraneTypeName)
            circleShape.setOutlineColor(sf::Color::Yellow);
        else
            circleShape.setOutlineColor(simulationConfigUpdater_->getMembraneTypeColorMap().at(type.getName()));

        circleShape.setRadius(R);
        circleShape.setOrigin({R, R});
        circleShape.setFillColor(sf::Color::Transparent);
        circleShape.setOutlineThickness(1);

        membraneTypeShapes_[ID] = circleShape;
    }
}

void SimulationWidget::queueFrameForRendering(Frame frame)
{
    frame_ = std::move(frame);
}

void SimulationWidget::renderFrameImmediately(Frame frame)
{
    queueFrameForRendering(std::move(frame));

    // Use the next draw event, otherwise we'll get a huge FPS increase that could crash the GUI if the user
    // drags the view and causes continuous redraws while the simulation is running
    if (renderingTimer_.isActive())
        return;

    drawFrame();
}

void SimulationWidget::drawFrame()
{
    using namespace std::chrono;
    const auto start = myClock::now();
    sf::RenderWindow::clear(sf::Color::Black);

    for (const auto& disc : frame_.discs)
    {
        discTypeShapes_[disc.getTypeID()].setPosition(utility::toVector2f(disc.getPosition()));
        sf::RenderWindow::draw(discTypeShapes_[disc.getTypeID()]);
    }

    for (const auto& membrane : frame_.membranes)
    {
        auto& membraneTypeShape = membraneTypeShapes_[membrane.getTypeID()];
        membraneTypeShape.setOutlineThickness(static_cast<float>(QSFMLWidget::getCurrentZoom()));
        membraneTypeShape.setPosition(utility::toVector2f(membrane.getPosition()));
        sf::RenderWindow::draw(membraneTypeShape);
    }

    sf::RenderWindow::display();

    if (!renderingTimer_.isActive())
        return;

    const auto now = myClock::now();
    const auto renderTime = now - start;

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

template <typename ObjectType, typename ObjectsGetter, typename NameSetter, typename ObjectsSetter>
void SimulationWidget::addObjectAtCursor(const QPoint& cursorPosition, const std::string& typeName,
                                         ObjectsGetter getObjects, NameSetter setTypeName, ObjectsSetter setObjects)
{
    const sf::Vector2f worldCoordinates = mapPixelToCoords(sf::Vector2i{cursorPosition.x(), cursorPosition.y()});

    auto config = simulationConfigUpdater_->getSimulationConfig();

    ObjectType object{};
    setTypeName(object, typeName);
    object.x = static_cast<double>(worldCoordinates.x);
    object.y = static_cast<double>(worldCoordinates.y);

    auto objects = getObjects(config);
    objects.push_back(std::move(object));
    setObjects(config, std::move(objects));

    try
    {
        simulationConfigUpdater_->setSimulationConfig(config);
    }
    catch (const std::exception& exception)
    {
        QMessageBox::critical(this, "Error", exception.what());
    }
}

void SimulationWidget::addDiscAtCursor(const QPoint& cursorPosition, const std::string& typeName)
{
    addObjectAtCursor<cell::config::Disc>(
        cursorPosition, typeName, [](const auto& config) { return config.discs; },
        [](auto& disc, const std::string& typeName) { disc.discTypeName = typeName; },
        [](auto& config, auto discs) { config.discs = std::move(discs); });
}

void SimulationWidget::addMembraneAtCursor(const QPoint& cursorPosition, const std::string& typeName)
{
    addObjectAtCursor<cell::config::Membrane>(
        cursorPosition, typeName, [](const auto& config) { return config.membranes; },
        [](auto& membrane, const std::string& typeName) { membrane.membraneTypeName = typeName; },
        [](auto& config, auto membranes) { config.membranes = std::move(membranes); });
}
