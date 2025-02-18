#ifndef SIMULATIONWIDGET_HPP
#define SIMULATIONWIDGET_HPP

#include "Disc.hpp"
#include "FrameDTO.hpp"
#include "QSFMLWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

class SimulationWidget : public QSFMLWidget
{
    Q_OBJECT
public:
    SimulationWidget(QWidget* parent);

public slots:
    void render(const FrameDTO& frameDTO);
    void initialize(const std::vector<Disc>& discs);

private:
    void removeDestroyedDiscs(const std::vector<int>& indices);
    void changeDiscs(const std::vector<std::pair<int, DiscType>>& changedDiscsIndices);
    sf::CircleShape circleShapeFromDisc(const Disc& disc);

private:
    std::vector<sf::CircleShape> circles_;
};

#endif /* SIMULATIONWIDGET_HPP */
