#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "FrameDTO.hpp"
#include "World.hpp"

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
    void collisionData(int collisions);
    void sceneData(const std::vector<Disc>& discs);

private:
    World world_;
};

#endif /* SIMULATION_HPP */
