#include "core/Simulation.hpp"
#include "cell/Cell.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "core/Utility.hpp"

#include <QThread>
#include <QTimer>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
{
}

void Simulation::run()
{
    using clock = std::chrono::steady_clock;
    using namespace std::chrono;

    auto lastUpdate = clock::now();
    auto timeSinceLastUpdate = duration<double>(0s);
    auto start = clock::now();
    auto simulationUpdateTime = duration<double>(0s);
    int updates = 0;

    // Since every change to the config causes an immediate context rebuild, it's always up to date
    const auto& simulationConfig = simulationConfigUpdater_.getSimulationConfig();
    const double simulationTimeScale = simulationConfig.simulationTimeScale;
    const auto simulationTimeStep = duration<double>(simulationConfig.simulationTimeStep);

    const auto emitSimulationData = [&]()
    {
        const auto elapsed = clock::now() - start;
        if (elapsed < 1s || updates == 0)
            return;

        const double simulationTime = updates * simulationConfig.simulationTimeStep;
        const double elapsedSeconds = duration<double>(elapsed).count();
        const double actualScale = simulationTime / elapsedSeconds;
        emit simulationData(simulationTimeScale, actualScale, duration_cast<nanoseconds>(elapsed / updates),
                            duration_cast<nanoseconds>(simulationUpdateTime / updates));

        start = clock::now();
        updates = 0;
        simulationUpdateTime = 0s;
    };

    while (!QThread::currentThread()->isInterruptionRequested())
    {
        auto now = clock::now();
        timeSinceLastUpdate += now - lastUpdate;
        lastUpdate = now;

        emitSimulationData();

        while (timeSinceLastUpdate / simulationTimeScale > simulationTimeStep &&
               !QThread::currentThread()->isInterruptionRequested())
        {
            timeSinceLastUpdate -= simulationTimeStep / simulationTimeScale;

            const auto updateStart = clock::now();
            simulationFactory_.getCell().update(simulationTimeStep.count());
            simulationUpdateTime += clock::now() - updateStart;
            ++updates;

            emitSimulationData();
            emitFrame(RedrawOnly{false});
        }
    }
}

void Simulation::buildContext(const cell::SimulationConfig& config)
{
    simulationFactory_.buildSimulationFromConfig(config);
}

void Simulation::rebuildContext()
{
    buildContext(simulationConfigUpdater_.getSimulationConfig());
}

const cell::DiscTypeRegistry& Simulation::getDiscTypeRegistry()
{
    return simulationFactory_.getSimulationContext().discTypeRegistry;
}

SimulationConfigUpdater& Simulation::getSimulationConfigUpdater()
{
    return simulationConfigUpdater_;
}

bool Simulation::cellIsBuilt() const
{
    return simulationFactory_.cellIsBuilt();
}

void Simulation::emitFrame(RedrawOnly redrawOnly)
{
    if (!simulationFactory_.cellIsBuilt())
        return;

    FrameDTO frameDTO;
    const auto& cell = simulationFactory_.getCell();

    const auto gatherData = [&](const cell::Compartment& compartment)
    {
        frameDTO.discs_.insert(frameDTO.discs_.end(), compartment.getDiscs().begin(), compartment.getDiscs().end());
        frameDTO.membranes_.push_back(circleShapeFromCompartment(compartment));
    };

    std::vector<const cell::Compartment*> compartments({&cell});

    while (!compartments.empty())
    {
        const cell::Compartment* compartment = compartments.back();
        compartments.pop_back();

        gatherData(*compartment);

        for (const auto& subCompartment : compartment->getCompartments())
            compartments.push_back(subCompartment.get());
    }

    if (redrawOnly.value)
    {
        emit frame(frameDTO);
        return;
    }

    frameDTO.elapsedSimulationTimeUs =
        static_cast<long long>(simulationConfigUpdater_.getSimulationConfig().simulationTimeStep * 1e6);
    frameDTO.collisionCounts_ = simulationFactory_.getAndResetCollisionCounts();

    emit frame(frameDTO);
}

sf::CircleShape Simulation::circleShapeFromCompartment(const cell::Compartment& compartment)
{
    sf::CircleShape shape;
    const auto& membraneTypeRegistry = simulationFactory_.getSimulationContext().membraneTypeRegistry;
    const auto& membraneType = membraneTypeRegistry.getByID(compartment.getMembrane().getTypeID());
    const auto R = static_cast<float>(membraneType.getRadius());

    shape.setPointCount(100);
    shape.setRadius(R);
    shape.setOrigin({R, R});
    shape.setPosition(utility::toVector2f(compartment.getMembrane().getPosition()));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(1);

    if (membraneType.getName() == cell::config::cellMembraneTypeName)
        shape.setOutlineColor(sf::Color::Yellow);
    else
        shape.setOutlineColor(simulationConfigUpdater_.getMembraneTypeColorMap().at(membraneType.getName()));

    return shape;
}
