#ifndef SIMULATIONWIDGET_HPP
#define SIMULATIONWIDGET_HPP

#include "core/FrameDTO.hpp"
#include "widgets/QSFMLWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

class AbstractSimulationBuilder;

/**
 * @brief Widget displaying the simulation state by drawing a bunch of circles in different colors each frame based on
 * what is currently in the simulation
 */
class SimulationWidget : public QSFMLWidget
{
    Q_OBJECT
public:
    SimulationWidget(QWidget* parent);

    void injectAbstractSimulationBuilder(AbstractSimulationBuilder* abstractSimulationBuilder);

    void closeEvent(QCloseEvent* event) override;

signals:
    void requestExitFullscreen();

public slots:
    /**
     * @brief Clears the render window and displays circles based on the given `FrameDTO`
     */
    void render(const FrameDTO& frame, const cell::DiscTypeResolver& discTypeResolver,
                const std::map<std::string, sf::Color>& colorMap);

private:
    std::vector<sf::CircleShape> circles_;
    sf::Clock clock_;
    sf::RectangleShape boundingRect_;
    AbstractSimulationBuilder* abstractSimulationBuilder_ = nullptr;
};

#endif /* SIMULATIONWIDGET_HPP */
