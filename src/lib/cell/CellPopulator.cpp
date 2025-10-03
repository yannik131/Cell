#include "CellPopulator.hpp"
#include "Cell.hpp"
#include "Compartment.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"

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
    const auto& discTypes = simulationConfig_.discTypes;
    double maxRadius = std::max_element(discTypes.begin(), discTypes.end(),
                                        [](const config::DiscType& lhs, const config::DiscType& rhs)
                                        { return lhs.radius < rhs.radius; })
                           ->radius;

    if (simulationConfig_.setup.distributions.empty())
        return;

    if (!simulationConfig_.setup.distributions.contains(""))
        throw ExceptionWithLocation("No default distribution available for cell population");
}

void CellPopulator::populateDirectly()
{
    std::vector<Disc> discs;

    for (const auto& disc : simulationConfig_.setup.discs)
    {
        Disc newDisc(simulationContext_.discTypeRegistry.getIDFor(disc.discTypeName));
        newDisc.setPosition({disc.x, disc.y});
        newDisc.setVelocity({disc.vx, disc.vy});

        discs.push_back(std::move(newDisc));
    }
}

/*std::vector<Disc> SimulationFactory::createDiscGridFromDistribution(const SimulationConfig& simulationConfig_,
                                                                    double maxRadius) const
{

    if (double total = calculateDistributionSum(simulationConfig_.setup.distribution); std::abs(total - 1) > 1e-3)
    {
        throw ExceptionWithLocation("Percentages for disc type distribution don't add up to 100%. They add up to " +
                                    std::to_string(total));
    }

    std::vector<Disc> discs;
    std::random_device rd;
    std::mt19937 gen(rd());

    throwIfNotInRange(simulationConfig_.setup.maxVelocity, SettingsLimits::MinMaxVelocity,
                      SettingsLimits::MaxMaxVelocity, "Max. velocity");
    std::uniform_real_distribution<double> velocityDistribution(-simulationConfig_.setup.maxVelocity,
                                                                simulationConfig_.setup.maxVelocity);

    discs.reserve(simulationConfig_.setup.discCount);

    std::vector<sf::Vector2d> discPositions =
        mathutils::calculateGrid(simulationConfig_.setup.cellWidth, simulationConfig_.setup.cellHeight, maxRadius);

    if (simulationConfig_.setup.discCount > static_cast<int>(discPositions.size()))
    {
        LOG(WARNING) << "According to the settings, " << std::to_string(simulationConfig_.setup.discCount)
                     << " discs should be created, but the grid can only fit " << std::to_string(discPositions.size())
                     << ". " << std::to_string(simulationConfig_.setup.discCount - discPositions.size())
                     << " discs will not be created.";
    }

    // We need the accumulated percentages sorted in ascending order for the random number approach to work
    std::vector<std::pair<DiscTypeID, double>> discTypes;
    for (const auto& pair : simulationConfig_.setup.distribution)
    {
        DiscTypeID ID = discTypeRegistry_->getIDFor(pair.first);
        discTypes.emplace_back(ID, pair.second + (discTypes.empty() ? 0 : discTypes.back().second));
    }

    std::ranges::sort(discTypes, [](const auto& a, const auto& b) { return a.second < b.second; });

    for (int i = 0; i < simulationConfig_.setup.discCount && !discPositions.empty(); ++i)
    {
        auto randomNumber = mathutils::getRandomNumber<double>(0, 1);

        for (const auto& [discType, percentage] : discTypes)
        {
            if (randomNumber < percentage)
            {
                Disc newDisc(discType);
                newDisc.setPosition(discPositions.back());
                newDisc.setVelocity(sf::Vector2d(velocityDistribution(gen), velocityDistribution(gen)));

                discs.push_back(newDisc);
                discPositions.pop_back();

                break;
            }
        }
    }

    return discs;
}*/

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

            if (mathutils::circlesIntersect(gridPoints[i], maxRadius, M, R))
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

} // namespace cell