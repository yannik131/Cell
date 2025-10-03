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
    if (simulationConfig_.discTypes.empty())
    {
        if (!simulationConfig_.setup.useDistribution && !simulationConfig_.setup.discs.empty())
            throw ExceptionWithLocation("Can't create discs directly: No disc types were specified");

        if (simulationConfig_.setup.useDistribution &&
            (simulationConfig_.setup.discCounts.empty() || simulationConfig_.setup.distributions.empty()))
            throw ExceptionWithLocation(
                "Can't create discs from distributions: Disc types and/or disc counts or distributions are empty");

        return;
    }

    if (simulationConfig_.setup.useDistribution)
        populateWithDistributions();
    else
        populateDirectly();
}

void CellPopulator::populateWithDistributions()
{
    if (simulationConfig_.setup.distributions.empty())
        return;

    if (!simulationConfig_.setup.distributions.contains(""))
        throw ExceptionWithLocation("No default distribution available for cell population");

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

std::vector<sf::Vector2d> CellPopulator::calculateCompartmentGridPoints(Compartment& compartment, double maxRadius)
{
    const auto& membraneType =
        simulationContext_.membraneTypeRegistry.getByID(compartment.getMembrane().getMembraneTypeID());
    const auto& membraneCenter = compartment.getMembrane().getPosition();
    const auto& membraneRadius = membraneType.getRadius();

    auto gridPoints = mathutils::calculateGrid(membraneRadius, membraneRadius, maxRadius);

    const auto& topLeft = membraneCenter - sf::Vector2d{membraneRadius, membraneRadius};
    for (size_t i = 0; i < gridPoints.size();)
    {
        gridPoints[i] += topLeft;
        bool valid = true;

        if (!mathutils::circleIsFullyContainedByCircle(gridPoints[i], maxRadius, membraneCenter, membraneRadius))
            valid = false;

        for (auto i = 0; valid && i < compartment.getCompartments().size();)
        {
            const auto& membrane = compartment.getCompartments()[i].getMembrane();
            const auto& M = membrane.getPosition();
            const auto& R = simulationContext_.membraneTypeRegistry.getByID(membrane.getMembraneTypeID()).getRadius();

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

    return gridPoints;
}

void CellPopulator::populateCompartmentWithDistribution(Compartment& compartment, double maxRadius)
{
    auto gridPoints = calculateCompartmentGridPoints(compartment, maxRadius);

    const auto& membrane = compartment.getMembrane();
    const auto& membraneType = simulationContext_.membraneTypeRegistry.getByID(membrane.getMembraneTypeID());

    const auto& discCount = simulationConfig_.setup.discCounts[membraneType.getName()];
    if (gridPoints.size() < discCount)
    {
        LOG(WARNING) << "According to the settings, " << std::to_string(discCount)
                     << " discs should be created for membranes of type\"" + membraneType.getName() +
                            "\", but the grid can only fit "
                     << std::to_string(gridPoints.size()) << ". " << std::to_string(discCount - gridPoints.size())
                     << " discs will not be created.";
    }

    std::vector<std::pair<DiscTypeID, double>> discTypes;
    for (const auto& pair : simulationConfig_.setup.distributions[membraneType.getName()])
    {
        DiscTypeID ID = simulationContext_.discTypeRegistry.getIDFor(pair.first);
        discTypes.emplace_back(ID, pair.second + (discTypes.empty() ? 0 : discTypes.back().second));
    }

    // We need the accumulated percentages sorted in ascending order for the random number approach to work
    std::sort(discTypes.begin(), discTypes.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

    for (int i = 0; i < discCount && !gridPoints.empty(); ++i)
    {
        auto randomNumber = mathutils::getRandomNumber<double>(0, 1);

        for (const auto& [discType, percentage] : discTypes)
        {
            if (randomNumber < percentage)
            {
                Disc newDisc(discType);
                newDisc.setPosition(gridPoints.back());
                newDisc.setVelocity(sampleVelocityFromDistribution());

                compartment.addDisc(std::move(newDisc));
                gridPoints.pop_back();

                break;
            }
        }
    }

    for (auto& subCompartment : compartment.getCompartments())
        populateCompartmentWithDistribution(compartment, maxRadius);
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
    const auto& R = simulationContext_.discTypeRegistry.getByID(disc.getDiscTypeID()).getRadius();

    auto isFullyContainedIn = [&](const Compartment& compartment)
    {
        const auto& membrane = compartment.getMembrane();
        const auto& membraneType = simulationContext_.membraneTypeRegistry.getByID(membrane.getMembraneTypeID());

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
            if (isFullyContainedIn(subCompartment))
            {
                compartment = &subCompartment;
                continueSearch = true;
            }
        }
    }

    return *compartment;
}

} // namespace cell