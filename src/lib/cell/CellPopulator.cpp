#include "CellPopulator.hpp"
#include "Cell.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"

#include <glog/logging.h>

#include <algorithm>
#include <numbers>
#include <random>
#include <vector>

namespace cell
{

CellPopulator::CellPopulator(Cell& cell, SimulationConfig simulationConfig, SimulationContext simulationContext)
    : cell_(cell)
    , simulationConfig_(std::move(simulationConfig))
    , simulationContext_(std::move(simulationContext))
{
}

void CellPopulator::populateCell()
{
    if (simulationConfig_.setup.useDistribution)
        populateWithDistributions();
    else
        populateDirectly();
}

void CellPopulator::populateWithDistributions()
{
    if (simulationConfig_.setup.distributions.empty())
        return;

    const auto& discTypes = simulationConfig_.discTypes;
    double maxRadius = std::max_element(discTypes.begin(), discTypes.end(),
                                        [](const config::DiscType& lhs, const config::DiscType& rhs)
                                        { return lhs.radius < rhs.radius; })
                           ->radius;

    populateCompartmentWithDistribution(cell_, maxRadius);
}

void CellPopulator::populateDirectly()
{
    for (const auto& disc : simulationConfig_.setup.discs)
    {
        Disc newDisc(simulationContext_.discTypeRegistry.getIDFor(disc.discTypeName));
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

std::vector<sf::Vector2d> CellPopulator::calculateCompartmentGridPoints(Compartment& compartment, double maxRadius,
                                                                        int discCount) const
{
    const auto& membraneType = simulationContext_.membraneTypeRegistry.getByID(compartment.getMembrane().getTypeID());
    const auto& membraneCenter = compartment.getMembrane().getPosition();
    const auto& membraneRadius = membraneType.getRadius();

    auto gridPoints = mathutils::calculateGrid(2 * membraneRadius, 2 * membraneRadius, 2 * maxRadius);

    const auto& topLeft = membraneCenter - sf::Vector2d{membraneRadius, membraneRadius};
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
            const auto& R = simulationContext_.membraneTypeRegistry.getByID(membrane.getTypeID()).getRadius();

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

    if (gridPoints.size() < discCount)
    {
        LOG(WARNING) << std::to_string(discCount)
                     << " discs should be created for membrane of type \"" + membraneType.getName() +
                            "\", but the grid can only fit "
                     << std::to_string(gridPoints.size()) << ". " << std::to_string(discCount - gridPoints.size())
                     << " discs will not be created.";
    }

    return gridPoints;
}

void CellPopulator::populateCompartmentWithDistribution(Compartment& compartment, double maxRadius)
{
    const auto& membraneTypeName =
        simulationContext_.membraneTypeRegistry.getByID(compartment.getMembrane().getTypeID()).getName();

    if (!simulationConfig_.setup.discCounts.contains(membraneTypeName))
        throw ExceptionWithLocation("No disc counts for membrane type " + membraneTypeName);

    if (!simulationConfig_.setup.distributions.contains(membraneTypeName))
        throw ExceptionWithLocation("No distribution for membrane type " + membraneTypeName);

    const auto& discCount = simulationConfig_.setup.discCounts[membraneTypeName];
    auto gridPoints = calculateCompartmentGridPoints(compartment, maxRadius, discCount);
    const auto& distribution = simulationConfig_.setup.distributions[membraneTypeName];

    if (double sum = calculateValueSum(distribution) * 100; std::abs(sum - 100) > 1e-1)
        throw ExceptionWithLocation("Distribution for membrane type " + membraneTypeName +
                                    " doesn't add up to 100%, it adds up to " + std::to_string(sum));

    for (const auto& [discTypeName, frequency] : distribution)
    {
        const auto count = static_cast<int>(std::round(frequency * discCount));
        const auto discTypeID = simulationContext_.discTypeRegistry.getIDFor(discTypeName);

        for (int i = 0; i < count && !gridPoints.empty(); ++i)
        {
            Disc newDisc(discTypeID);
            newDisc.setPosition(gridPoints.back());
            newDisc.setVelocity(sampleVelocityFromDistribution());

            compartment.addDisc(std::move(newDisc));
            gridPoints.pop_back();
        }
    }

    for (auto& subCompartment : compartment.getCompartments())
        populateCompartmentWithDistribution(*subCompartment, maxRadius);
}

sf::Vector2d CellPopulator::sampleVelocityFromDistribution() const
{
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());

    const auto sigma = simulationConfig_.setup.maxVelocity;

    // 2D maxwell distribution is a rayleigh distribution
    // weibull distribution with k=2 and lambda = sigma*sqrt(2) is rayleigh distribution

    std::uniform_real_distribution<double> angleDistribution(0, 2 * std::numbers::pi);
    std::weibull_distribution<double> speedDistribution(2, std::numbers::sqrt2 * sigma);

    const auto angle = angleDistribution(gen);
    const auto speed = speedDistribution(gen);

    return sf::Vector2d{speed * std::cos(angle), speed * std::sin(angle)};
}

Compartment& CellPopulator::findDeepestContainingCompartment(const Disc& disc)
{
    const auto& M = disc.getPosition();
    const auto& R = simulationContext_.discTypeRegistry.getByID(disc.getTypeID()).getRadius();

    auto isFullyContainedIn = [&](const Compartment& compartment)
    {
        const auto& membrane = compartment.getMembrane();
        const auto& membraneType = simulationContext_.membraneTypeRegistry.getByID(membrane.getTypeID());

        return mathutils::circleIsFullyContainedByCircle(M, R, membrane.getPosition(), membraneType.getRadius());
    };

    if (!isFullyContainedIn(cell_))
        throw ExceptionWithLocation("Disc at (" + std::to_string(M.x) + ", " + std::to_string(M.y) +
                                    ") is not contained by the cell");

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

double CellPopulator::calculateValueSum(const std::map<std::string, double>& distribution) const
{
    return std::accumulate(distribution.begin(), distribution.end(), 0.0,
                           [](double partialSum, const auto& entry) { return partialSum + entry.second; });
}

} // namespace cell