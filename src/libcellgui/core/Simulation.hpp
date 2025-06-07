#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "Cell.hpp"
#include "FrameDTO.hpp"

#include <QObject>
#include <SFML/System/Time.hpp>

#include <vector>

class Simulation : public QObject
{
    Q_OBJECT
public:
    explicit Simulation(QObject* parent = nullptr);
    void run();
    void reset();
    void setWorldBounds(const sf::Vector2f& bounds);

private:
    void emitFrameData();

signals:
    void frameData(const FrameDTO& data);
    void sceneData(const std::vector<Disc>& discs);

private:
    Cell world_;
    const std::vector<Disc>& worldDiscs_;
};

#endif /* SIMULATION_HPP */
