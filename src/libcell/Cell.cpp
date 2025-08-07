#include "Cell.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "PositionNanoflannAdapter.hpp"
#include "Reactions.hpp"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <execution>
#include <map>
#include <random>
#include <set>

namespace cell
{

Cell::Cell() = default;

template <typename T> DiscType::map<T> operator+=(DiscType::map<T>& a, const DiscType::map<T>& b)
{
    for (const auto& [key, value] : b)
        a[key] += value;

    return a;
}

void Cell::update(const sf::Time& dt)
{
    auto width = static_cast<float>(GlobalSettings::getSettings().cellWidth_);
    auto height = static_cast<float>(GlobalSettings::getSettings().cellHeight_);

    newDiscs_.clear();

    for (auto& disc : discs_)
    {
        disc.move(disc.getVelocity() * dt.asSeconds());
        currentKineticEnergy_ += mathutils::handleWorldBoundCollision(disc, {0, 0}, {width, height},
                                                                      initialKineticEnergy_ - currentKineticEnergy_);
    }

    const auto& newDiscs = unimolecularReactions(discs_);
    newDiscs_.insert(newDiscs_.end(), newDiscs.begin(), newDiscs.end());
    discs_.insert(discs_.end(), newDiscs_.begin(), newDiscs_.end());

    const auto& collidingDiscs = mathutils::findCollidingDiscs(discs_, maxRadius_);
    collisionCounts_ += mathutils::handleDiscCollisions(collidingDiscs);

    removeDestroyedDiscs();
}

DiscType::map<int> Cell::getAndResetCollisionCount()
{
    auto tmp = std::move(collisionCounts_);
    collisionCounts_.clear();

    return tmp;
}

const std::vector<Disc>& Cell::getDiscs() const
{
    return discs_;
}

void Cell::reinitialize()
{
    discs_.clear();
    startPositions_.clear();

    initializeStartPositions();
    buildScene();
}

double Cell::getInitialKineticEnergy() const
{
    return initialKineticEnergy_;
}

double Cell::getCurrentKineticEnergy() const
{
    return currentKineticEnergy_;
}

void Cell::buildScene()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 100);
    std::uniform_real_distribution<double> velocityDistribution(-600., 600.);
    const auto& settings = GlobalSettings::getSettings();

    discs_.reserve(settings.numberOfDiscs_);
    DiscType::map<int> counts;

    if (settings.numberOfDiscs_ > static_cast<int>(startPositions_.size()))
        LOG(WARNING) << "According to the settings, " << std::to_string(settings.numberOfDiscs_)
                     << " discs should be created, but the render window can only fit "
                     << std::to_string(startPositions_.size()) << ". "
                     << std::to_string(settings.numberOfDiscs_ - startPositions_.size())
                     << " discs will not be created.";

    // We need the accumulated percentages sorted in ascending order for the random number approach to work
    std::vector<std::pair<DiscType, int>> discTypes;
    for (const auto& pair : settings.discTypeDistribution_)
        discTypes.emplace_back(pair.first, pair.second + (discTypes.empty() ? 0 : discTypes.back().second));

    std::ranges::sort(discTypes, [](const auto& a, const auto& b) { return a.second < b.second; });

    initialKineticEnergy_ = 0.;
    for (int i = 0; i < settings.numberOfDiscs_ && !startPositions_.empty(); ++i)
    {
        int randomNumber = distribution(gen);

        for (const auto& [discType, percentage] : discTypes)
        {
            if (randomNumber < percentage || percentage == 100)
            {
                counts[discType]++;
                Disc newDisc(discType);
                newDisc.setPosition(startPositions_.back());
                newDisc.setVelocity(sf::Vector2d(velocityDistribution(gen), velocityDistribution(gen)));
                initialKineticEnergy_ += newDisc.getKineticEnergy();

                discs_.push_back(newDisc);
                startPositions_.pop_back();

                break;
            }
        }
    }

    currentKineticEnergy_ = initialKineticEnergy_;

    DLOG(INFO) << "Radius distribution";
    for (const auto& [discType, count] : counts)
    {
        DLOG(INFO) << discType.getName() << " (" + std::to_string(discType.getRadius()) + "px): " << count << "/"
                   << settings.numberOfDiscs_ << " ("
                   << static_cast<float>(count) / static_cast<float>(settings.numberOfDiscs_) * 100 << "%)\n";
    }
}

void Cell::initializeStartPositions()
{
    auto width = static_cast<float>(GlobalSettings::getSettings().cellWidth_);
    auto height = static_cast<float>(GlobalSettings::getSettings().cellHeight_);

    startPositions_.reserve(static_cast<std::size_t>((width / maxRadius_) * (height / maxRadius_)));
    double spacing = maxRadius_ + 1;

    for (int i = 0; i < static_cast<int>(width / (2 * spacing)); ++i)
    {
        for (int j = 0; j < static_cast<int>(height / (2 * spacing)); ++j)
        {
            startPositions_.emplace_back(spacing * static_cast<float>(2 * i + 1),
                                         spacing * static_cast<float>(2 * j + 1));
        }
    }

    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(startPositions_.begin(), startPositions_.end(), g);
}

void Cell::removeDestroyedDiscs()
{
    currentKineticEnergy_ = 0.;
    for (auto iter = discs_.begin(); iter != discs_.end();)
    {
        if (iter->isMarkedDestroyed())
            iter = discs_.erase(iter);
        else
        {
            currentKineticEnergy_ += iter->getKineticEnergy();
            ++iter;
        }
    }
}

} // namespace cell