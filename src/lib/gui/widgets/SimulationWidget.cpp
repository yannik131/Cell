#include "widgets/SimulationWidget.hpp"
#include "core/AbstractSimulationBuilder.hpp"

#include <QCloseEvent>
#include <QLayout>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <glog/logging.h>

#include <map>

SimulationWidget::SimulationWidget(QWidget* parent)
    : QSFMLWidget(parent)
{
    boundingRect_.setOutlineColor(sf::Color::Yellow);
    boundingRect_.setOutlineThickness(1);
    boundingRect_.setFillColor(sf::Color::Transparent);
}

void SimulationWidget::injectAbstractSimulationBuilder(AbstractSimulationBuilder* abstractSimulationBuilder)
{
    abstractSimulationBuilder_ = abstractSimulationBuilder;
    abstractSimulationBuilder->registerConfigObserver(
        [&](const cell::SimulationConfig& config, const std::map<std::string, sf::Color>&)
        {
            boundingRect_.setSize(
                sf::Vector2f{static_cast<float>(config.setup.cellWidth), static_cast<float>(config.setup.cellHeight)});
        });
}

void SimulationWidget::closeEvent(QCloseEvent* event)
{
    // If the widget is full screen, exit full screen instead of destroying it
    emit requestExitFullscreen();
    event->ignore();
}

void SimulationWidget::toggleFullscreen()
{
    // This was ChatGPT-generated, so it's still pretty ugly.
    // TODO Fix fullscreen issue on window, repeated calls to this don't fill out the entire screen

    static QWidget* origParent = nullptr;
    static QLayout* origLayout = nullptr;
    static QWidget* placeholder = nullptr;
    static Qt::WindowFlags origFlags;

    if (!placeholder)
    {
        // Detach to full screen
        origParent = parentWidget();
        origLayout = origParent ? origParent->layout() : nullptr;
        origFlags = windowFlags();

        if (origLayout)
        {
            placeholder = new QWidget(origParent);
            placeholder->setSizePolicy(sizePolicy());
            origLayout->replaceWidget(this, placeholder);
        }

        setParent(nullptr);
        setWindowFlag(Qt::Window, true);
        showFullScreen();
        raise();
        activateWindow();
    }
    else
    {
        // Restore to original place
        showNormal(); // leave full screen
        setWindowFlags(origFlags & ~Qt::Window);
        setParent(origParent);

        if (origLayout)
        {
            origLayout->replaceWidget(placeholder, this);
            placeholder->deleteLater();
        }

        placeholder = nullptr;
        origParent = nullptr;
        origLayout = nullptr;

        show(); // apply new flags/parent
    }
}

void SimulationWidget::render(const FrameDTO& frame, const cell::DiscTypeResolver& discTypeResolver,
                              const std::map<std::string, sf::Color>& colorMap)
{
    const int FPS = 120;
    if (frame.elapsedSimulationTimeUs > 0 && clock_.getElapsedTime() < sf::seconds(1.f / FPS))
        return;

    clock_.restart();

    sf::RenderWindow::clear(sf::Color::Black);
    sf::CircleShape circleShape;

    for (const auto& disc : frame.discs_)
    {
        const auto& discType = discTypeResolver(disc.getDiscTypeID());

        circleShape.setPosition(static_cast<sf::Vector2f>(disc.getPosition()));
        circleShape.setRadius(discType.getRadius());
        circleShape.setFillColor(colorMap.at(discType.getName()));
        circleShape.setOrigin(
            sf::Vector2f{static_cast<float>(discType.getRadius()), static_cast<float>(discType.getRadius())});

        sf::RenderWindow::draw(circleShape);
    }

    boundingRect_.setOutlineThickness(static_cast<float>(QSFMLWidget::getCurrentZoom()));

    sf::RenderWindow::draw(boundingRect_);
    sf::RenderWindow::display();
}
