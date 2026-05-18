#include "CellPopulator.hpp"
#include "Cell.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"

#include <algorithm>
#include <iostream>
#include <numbers>
#include <random>
#include <vector>

namespace cell
{

CellPopulator::CellPopulator(Cell& cell, SimulationConfig simulationConfig, const DiscTypeRegistry& discTypeRegistry,
                             const MembraneTypeRegistry& membraneTypeRegistry)
    : cell_(cell)
    , simulationConfig_(std::move(simulationConfig))
    , discTypeRegistry_(discTypeRegistry)
    , membraneTypeRegistry_(membraneTypeRegistry)
{
}

void CellPopulator::populateCell()
{
    if (simulationConfig_.useDistribution)
        populateWithDistributions();
    else
        populateDirectly();
}

void CellPopulator::populateWithDistributions()
{
    const auto& discTypes = simulationConfig_.discTypes;
    if (discTypes.empty())
        return;

    double maxRadius = std::max_element(discTypes.begin(), discTypes.end(),
                                        [](const config::DiscType& lhs, const config::DiscType& rhs)
                                        { return lhs.radius < rhs.radius; })
                           ->radius;

    populateCompartmentWithDistribution(cell_, maxRadius);
}

void CellPopulator::populateDirectly()
{
    for (const auto& disc : simulationConfig_.discs)
    {
        Disc newDisc(discTypeRegistry_.getIDFor(disc.discTypeName));
        newDisc.setPosition({disc.x, disc.y});
        newDisc.setVelocity({disc.vx, disc.vy});

        auto& compartment = findDeepestContainingCompartment(newDisc);
        compartment.addDisc(std::move(newDisc));
    }
}

double CellPopulator::calculateDistributionSum(const std::map<std::string, double>& distribution) const
{
    return std::accumulate(distribution.begin(), distribution.end(), 0.0,
                           [](double currentSum, auto& entryPair) { return currentSum + entryPair.second; });
}

std::vector<Vector2d> CellPopulator::calculateCompartmentGridPoints(Compartment& compartment, double maxRadius,
                                                                    int discCount) const
{
    const auto& membraneType = membraneTypeRegistry_.getByID(compartment.getMembrane().getTypeID());
    const auto& membraneCenter = compartment.getMembrane().getPosition();
    const auto& membraneRadius = membraneType.getRadius();

    auto gridPoints = mathutils::calculateGrid(2 * membraneRadius, 2 * membraneRadius, 2 * maxRadius);

    const auto& topLeft = membraneCenter - Vector2d{membraneRadius, membraneRadius};
    for (size_t i = 0; i < gridPoints.size();)
    {
        gridPoints[i] += topLeft;
        bool valid = true;

        if (!mathutils::circleIsFullyContainedByCircle(gridPoints[i], maxRadius, membraneCenter, membraneRadius))
            valid = false;

        const auto& subCompartments = compartment.getCompartments();
        for (auto iter = subCompartments.begin(); valid && iter != subCompartments.end(); ++iter)
        {
            const auto& membrane = (*iter)->getMembrane();
            const auto& M = membrane.getPosition();
            const auto& R = membraneTypeRegistry_.getByID(membrane.getTypeID()).getRadius();

            if (mathutils::circlesOverlap(gridPoints[i], maxRadius, M, R))
                valid = false;
        }

        if (!valid)
        {
            std::swap(gridPoints[i], gridPoints.back());
            gridPoints.pop_back();
        }
        else
            ++i;
    }

    if (static_cast<int>(gridPoints.size()) < discCount)
    {
        std::cout << "Grid for \"" << membraneType.getName() << "\" can only fit " << gridPoints.size() << "/"
                  << discCount << " discs\n";
    }

    return gridPoints;
}

void CellPopulator::populateCompartmentWithDistribution(Compartment& compartment, double maxRadius)
{
    for (auto& subCompartment : compartment.getCompartments())
        populateCompartmentWithDistribution(*subCompartment, maxRadius);

    const auto& membraneTypeName = membraneTypeRegistry_.getByID(compartment.getMembrane().getTypeID()).getName();

    const auto& membraneType = findMembraneTypeByName(simulationConfig_, membraneTypeName);
    const auto& distribution = membraneType.discTypeDistribution;

    if (membraneType.discCount < 0)
        throw ExceptionWithLocation("Disc count for membrane type \"" + membraneTypeName + "\" is negative (" +
                                    std::to_string(membraneType.discCount) + ")");

    if (membraneType.discCount == 0 || distribution.empty())
        return;

    auto gridPoints = calculateCompartmentGridPoints(compartment, maxRadius, membraneType.discCount);
    const auto discCount = std::min(static_cast<std::size_t>(membraneType.discCount), gridPoints.size());

    if (!distribution.empty())
    {
        if (double sum = calculateValueSum(distribution) * 100; std::abs(sum - 100) > 1e-1)
            throw ExceptionWithLocation("Distribution for membrane type \"" + membraneTypeName +
                                        "\" doesn't add up to 100%, it adds up to " + std::to_string(sum) + "%");
    }

    for (const auto& [discTypeName, frequency] : distribution)
    {
        const auto count = static_cast<int>(std::round(frequency * static_cast<double>(discCount)));
        const auto discTypeID = discTypeRegistry_.getIDFor(discTypeName);
        const auto& discType = discTypeRegistry_.getByID(discTypeID);

        for (int i = 0; i < count && !gridPoints.empty(); ++i)
        {
            Disc newDisc(discTypeID);
            newDisc.setPosition(gridPoints.back());
            newDisc.setVelocity(
                sampleVelocityFromDistribution(simulationConfig_.mostProbableSpeed, discType.getMass()));

            compartment.addDisc(std::move(newDisc));
            gridPoints.pop_back();
        }
    }
}

Vector2d CellPopulator::sampleVelocityFromDistribution(double mostProbableSpeed, double m) const
{
    static thread_local std::mt19937 gen(std::random_device{}());
    std::normal_distribution<double> normalDistribution(0, mostProbableSpeed / std::sqrt(m));
    return Vector2d{normalDistribution(gen), normalDistribution(gen)};
}

Compartment& CellPopulator::findDeepestContainingCompartment(const Disc& disc)
{
    const auto& M = disc.getPosition();
    const auto& R = discTypeRegistry_.getByID(disc.getTypeID()).getRadius();

    auto isFullyContainedIn = [&](const Compartment& compartment)
    {
        const auto& membrane = compartment.getMembrane();
        const auto& membraneType = membraneTypeRegistry_.getByID(membrane.getTypeID());

        return mathutils::circleIsFullyContainedByCircle(M, R, membrane.getPosition(), membraneType.getRadius());
    };

    if (!isFullyContainedIn(cell_))
    {
        std::cout << "Disc at (" + std::to_string(M.x) + ", " + std::to_string(M.y) + ") is not contained by the cell";
        return cell_;
    }

    Compartment* compartment = &cell_;
    bool continueSearch = true;

    while (continueSearch)
    {
        continueSearch = false;
        for (auto& subCompartment : compartment->getCompartments())
        {
            if (isFullyContainedIn(*subCompartment))
            {
                compartment = subCompartment.get();
                continueSearch = true;
            }
        }
    }

    return *compartment;
}

double CellPopulator::calculateValueSum(const std::unordered_map<std::string, double>& distribution) const
{
    return std::accumulate(distribution.begin(), distribution.end(), 0.0,
                           [](double partialSum, const auto& entry) { return partialSum + entry.second; });
}

} // namespace cell