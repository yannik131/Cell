#include "CellState.hpp"
#include "Disc.hpp"
#include "ExceptionWithLocation.hpp"
#include "Settings.hpp"
#include "Vector2d.hpp"

#include <glog/logging.h>

#include <random>
#include <ranges>

namespace cell
{

namespace
{
std::random_device rd;
const std::mt19937 gen(rd());

template <typename T> int calculateValueSum(const std::unordered_map<T, int>& map)
{
    return std::accumulate(map.begin(), map.end(), 0,
                           [](int currentSum, auto& entryPair) { return currentSum + entryPair.second; });
}

/**
 * @brief Calculates a grid of starting positions for discs based on the largest radius of all disc types in the
 * settings. Doesn't check if the disc type with the largest radius can actually be created with the currently
 * defined set of reactions, though.
 */
std::vector<sf::Vector2d> calculateGrid(int width, int height, int edgeLength)
{
    std::vector<sf::Vector2d> gridPoints;
    gridPoints.reserve(static_cast<std::size_t>((width / edgeLength) * (height / edgeLength)));
    double spacing = edgeLength + 1;

    for (int i = 0; i < static_cast<int>(width / (2 * spacing)); ++i)
    {
        for (int j = 0; j < static_cast<int>(height / (2 * spacing)); ++j)
        {
            gridPoints.emplace_back(spacing * static_cast<double>(2 * i + 1), spacing * static_cast<double>(2 * j + 1));
        }
    }

    std::shuffle(gridPoints.begin(), gridPoints.end(), gen);

    return gridPoints;
}
} // namespace

CellState::CellState(DiscTypeResolver discTypeResolver)
    : discTypeResolver_(std::move(discTypeResolver))
{
}

void CellState::setNumberOfDiscs(int numberOfDiscs)
{
    throwIfNotInRange(numberOfDiscs, SettingsLimits::MinNumberOfDiscs, SettingsLimits::MaxNumberOfDiscs,
                      "number of discs");

    numberOfDiscs_ = numberOfDiscs;
}

void CellState::setCellWidth(int cellWidth)
{
    throwIfNotInRange(cellWidth, SettingsLimits::MinCellWidth, SettingsLimits::MaxCellWidth, "cell width");
    cellWidth_ = cellWidth;
}

void CellState::setCellHeight(int cellHeight)
{
    throwIfNotInRange(cellHeight, SettingsLimits::MinCellHeight, SettingsLimits::MaxCellHeight, "cell height");
    cellHeight_ = cellHeight;
}

void CellState::setDiscTypeDistribution(const DiscTypeMap<int> discTypeDistribution)
{
    if (discTypeDistribution.empty())
        throw ExceptionWithLocation("Disc type distribution cannot be empty");

    if (int totalPercent = calculateValueSum(discTypeDistribution); totalPercent != 100)
        throw ExceptionWithLocation("Percentages for disc type distribution don't add up to 100. They add up to " +
                                    std::to_string(totalPercent));

    discTypeDistribution_ = discTypeDistribution;
}

void CellState::randomizeUsingDiscTypeDistribution()
{
    if (discTypeDistribution_.empty())
        throw ExceptionWithLocation("Disc type distribution can't be empty");

    auto discTypeID = std::ranges::max_element(
                          discTypeDistribution_, [&](const auto& a, const auto& b)
                          { return discTypeResolver_(a.first).getRadius() < discTypeResolver_(b.first).getRadius(); })
                          ->first;

    maxRadius_ = discTypeResolver_(discTypeID).getRadius();

    discs_.clear();

    auto gridPositions = calculateGrid(cellWidth_, cellHeight_, maxRadius_);

    build(std::move(gridPositions));
}

void CellState::build(std::vector<sf::Vector2d> discPositions)
{
    std::uniform_int_distribution<int> distribution(0, 100);
    std::uniform_real_distribution<double> velocityDistribution(-600.0, 600.0);

    discs_.reserve(numberOfDiscs_);
    DiscTypeMap<int> counts;

    if (numberOfDiscs_ > static_cast<int>(discPositions.size()))
        LOG(WARNING) << "According to the settings, " << std::to_string(numberOfDiscs_)
                     << " discs should be created, but the render window can only fit "
                     << std::to_string(discPositions.size()) << ". "
                     << std::to_string(numberOfDiscs_ - discPositions.size()) << " discs will not be created.";

    // We need the accumulated percentages sorted in ascending order for the random number approach to work
    std::vector<std::pair<DiscTypeID, int>> discTypes;
    for (const auto& pair : discTypeDistribution_)
        discTypes.emplace_back(pair.first, pair.second + (discTypes.empty() ? 0 : discTypes.back().second));

    std::ranges::sort(discTypes, [](const auto& a, const auto& b) { return a.second < b.second; });

    initialKineticEnergy_ = 0.0;
    for (int i = 0; i < numberOfDiscs_ && !discPositions.empty(); ++i)
    {
        int randomNumber = distribution(gen);

        for (const auto& [discType, percentage] : discTypes)
        {
            if (randomNumber < percentage || percentage == 100)
            {
                counts[discType]++;
                Disc newDisc(discType);
                newDisc.setPosition(discPositions.back());
                newDisc.setVelocity(sf::Vector2d(velocityDistribution(gen), velocityDistribution(gen)));
                initialKineticEnergy_ += newDisc.getKineticEnergy(discTypeResolver_);

                discs_.push_back(newDisc);
                discPositions.pop_back();

                break;
            }
        }
    }

    currentKineticEnergy_ = initialKineticEnergy_;

    DLOG(INFO) << "Radius distribution";
    for (const auto& [discType, count] : counts)
    {
        DLOG(INFO) << discTypeResolver_(discType).getName()
                   << " (" + std::to_string(discTypeResolver_(discType).getRadius()) + "px): " << count << "/"
                   << numberOfDiscs_ << " (" << static_cast<float>(count) / static_cast<float>(numberOfDiscs_) * 100
                   << "%)\n";
    }
}

} // namespace cell