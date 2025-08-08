#include "CellState.hpp"
#include "Disc.hpp"
#include "GlobalSettings.hpp"

#include <glog/logging.h>

#include <random>
#include <ranges>

namespace cell
{

namespace
{

double findMaxDiscRadiusInSettings()
{
    const auto& discTypes = GlobalSettings::getSettings().discTypes_;
    if (discTypes.empty())
        throw ExceptionWithLocation("Can't find max disc radius: No disc types in settings.");

    return std::ranges::max_element(GlobalSettings::getSettings().discTypes_, {}, &DiscType::getRadius)->getRadius();
}

std::vector<sf::Vector2d> calculateGridStartPositions(int width, int height, int gridSize)
{
    std::vector<sf::Vector2d> startPositions;
    startPositions.reserve(static_cast<std::size_t>((width / gridSize) * (height / gridSize)));
    int spacing = gridSize + 1;

    for (int i = 0; i < static_cast<int>(width / (2 * spacing)); ++i)
    {
        for (int j = 0; j < static_cast<int>(height / (2 * spacing)); ++j)
        {
            startPositions.emplace_back(spacing * static_cast<double>(2 * i + 1),
                                        spacing * static_cast<double>(2 * j + 1));
        }
    }

    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(startPositions.begin(), startPositions.end(), g);

    return startPositions;
}

// We need the accumulated percentages sorted in ascending order for the random number approach to work
DiscType::map<int> getAccumulatedPercentagesFromSettings()
{
    std::vector<std::pair<DiscType*, int>> discTypes;
    for (const auto& pair : GlobalSettings::getSettings().discTypeDistribution_)
    {
        int accumulatedPercentage = discTypes.empty() ? 0 : discTypes.back().second;
        discTypes.emplace_back(pair.first, pair.second + accumulatedPercentage);
    }

    std::ranges::sort(discTypes, [](const auto& a, const auto& b) { return a.second < b.second; });
}

} // namespace

void CellState::addDisc(const Disc& disc)
{
    discs_.push_back(disc);
}

void CellState::addMembrane(const Membrane& membrane)
{
    membranes_.push_back(membrane);
}

void CellState::randomize()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> distribution(0, 100);
    static std::uniform_real_distribution<double> velocityDistribution(-600., 600.);

    const auto& settings = GlobalSettings::getSettings();

    discs_.reserve(settings.numberOfDiscs_);
    DiscType::map<int> counts;

    double maxDiscRadius = findMaxDiscRadiusInSettings();
    std::vector<sf::Vector2d> startPositions =
        calculateGridStartPositions(settings.cellWidth_, settings.cellHeight_, maxDiscRadius);

    if (settings.numberOfDiscs_ > static_cast<int>(startPositions.size()))
    {
        LOG(WARNING) << "According to the settings, " << std::to_string(settings.numberOfDiscs_)
                     << " discs should be created, but the start positions can only fit "
                     << std::to_string(startPositions.size()) << ". "
                     << std::to_string(settings.numberOfDiscs_ - startPositions.size())
                     << " discs will not be created.";
    }

    DiscType::map<int> discTypes = getAccumulatedPercentagesFromSettings();

    for (int i = 0; i < settings.numberOfDiscs_ && !startPositions.empty(); ++i)
    {
        int randomNumber = distribution(gen);

        for (const auto& [discType, percentage] : discTypes)
        {
            if (randomNumber < percentage || percentage == 100)
            {
                counts[discType]++;
                Disc newDisc(discType);
                newDisc.setPosition(startPositions.back());
                newDisc.setVelocity(sf::Vector2d(velocityDistribution(gen), velocityDistribution(gen)));

                discs_.push_back(newDisc);
                startPositions.pop_back();

                break;
            }
        }
    }
}

std::vector<Disc>& CellState::getDiscs()
{
    return discs_;
}

std::vector<Membrane>& CellState::getMembranes()
{
    return membranes_;
}

} // namespace cell