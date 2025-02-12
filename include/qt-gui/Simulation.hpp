#ifndef SIMULATION_H
#define SIMULATION_H

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

private:
    void emitFrameData();

signals:
    void frameData(const FrameDTO& data);
    void collisionData(int collisions);
    void sceneData(const std::vector<Disc>& discs);

private:
    const sf::Time SimulationTimeStep = sf::milliseconds(5);
    const sf::Time FrameTime = sf::milliseconds(1000 / 40);
    const sf::Time CollisionUpdateTime = sf::seconds(1);
    World world_;
};

#endif /* SIMULATION_H */
