#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "FrameDTO.hpp"
#include "World.hpp"

#include <QObject>
#include <QtCore/QMetaType>
#include <SFML/System/Time.hpp>

#include <vector>

Q_DECLARE_METATYPE(FrameDTO);

class Simulation : public QObject 
{
    Q_OBJECT
public:
    explicit Simulation(QObject* parent = nullptr);
    void run();
    void reset();

private:
    void emitFrameData();

signals:
    void frameData(const FrameDTO& data);
    void collisionData(int collisions);
    void sceneData(const std::vector<Disc>& discs);

private:
    sf::Time SimulationTimeStep = sf::milliseconds(5);
    sf::Time FrameTime = sf::milliseconds(1000 / 40);
    sf::Time CollisionUpdateTime = sf::seconds(1);
    World world_;
};

#endif /* SIMULATION_HPP */
