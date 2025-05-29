#include "World.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "NanoflannAdapter.hpp"
#include "Reactions.hpp"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <execution>
#include <map>
#include <random>
#include <set>

World::World() = default;

template <typename T> DiscType::map<T> operator+=(DiscType::map<T>& a, const DiscType::map<T>& b)
{
    for (const auto& [key, value] : b)
        a[key] += value;

    return a;
}

void World::update(const sf::Time& dt)
{
    destroyedDiscsIndices_.clear();
    newDiscs_.clear();

    for (auto& disc : discs_)
    {
        disc.move(disc.getVelocity() * dt.asSeconds());
        MathUtils::handleWorldBoundCollision(disc, {0, 0}, bounds_, initialKineticEnergy_ - currentKineticEnergy_);
    }

    const auto& newDiscs = unimolecularReactions(discs_);
    newDiscs_.insert(newDiscs_.end(), newDiscs.begin(), newDiscs.end());
    discs_.insert(discs_.end(), newDiscs_.begin(), newDiscs_.end());

    const auto& collidingDiscs = MathUtils::findCollidingDiscs(discs_, maxRadius_);
    collisionCounts_ += MathUtils::handleDiscCollisions(collidingDiscs);

    removeDestroyedDiscs();
}

DiscType::map<int> World::getAndResetCollisionCount()
{
    auto tmp = std::move(collisionCounts_);
    collisionCounts_.clear();

    return tmp;
}

const std::vector<Disc>& World::discs() const
{
    return discs_;
}

void World::reinitialize()
{
    const auto& discTypeDistribution = GlobalSettings::getSettings().discTypeDistribution_;

    maxRadius_ = std::ranges::max_element(discTypeDistribution, [](const auto& a, const auto& b)
                                          { return a.first.getRadius() < b.first.getRadius(); })
                     ->first.getRadius();

    discs_.clear();
    startPositions_.clear();

    initializeStartPositions();
    buildScene();
}

void World::setBounds(const sf::Vector2f& bounds)
{
    if (bounds.x <= 0 || bounds.y <= 0)
        throw ExceptionWithLocation("Bounds must be > 0");

    bounds_ = bounds;
}

float World::getInitialKineticEnergy() const
{
    return initialKineticEnergy_;
}

float World::getCurrentKineticEnergy() const
{
    return currentKineticEnergy_;
}

void World::buildScene()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 100);
    std::uniform_int_distribution<int> velocityDistribution(-600, 600);
    const auto& settings = GlobalSettings::getSettings();

    discs_.reserve(settings.numberOfDiscs_);
    std::map<int, int> counts;

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

    initialKineticEnergy_ = 0.f;
    for (int i = 0; i < settings.numberOfDiscs_ && !startPositions_.empty(); ++i)
    {
        int randomNumber = distribution(gen);

        for (const auto& [discType, percentage] : discTypes)
        {
            if (randomNumber < percentage || percentage == 100)
            {
                counts[discType.getRadius()]++;
                Disc newDisc(discType);
                newDisc.setPosition(startPositions_.back());
                newDisc.setVelocity(sf::Vector2f(velocityDistribution(gen), velocityDistribution(gen)));
                initialKineticEnergy_ += newDisc.getKineticEnergy();

                discs_.push_back(newDisc);
                startPositions_.pop_back();

                break;
            }
        }
    }

    currentKineticEnergy_ = initialKineticEnergy_;

    DLOG(INFO) << "Radius distribution";
    for (const auto& [radius, count] : counts)
    {
        DLOG(INFO) << radius << ": " << count << "/" << settings.numberOfDiscs_ << " ("
                   << count / static_cast<float>(settings.numberOfDiscs_) * 100 << "%)\n";
    }
}

void World::initializeStartPositions()
{
    if (bounds_.x == 0 || bounds_.y == 0)
        throw ExceptionWithLocation("Can't initialize world: Bounds not set");

    startPositions_.reserve((bounds_.x / maxRadius_) * (bounds_.y / maxRadius_));

    float spacing = maxRadius_ + 1;

    for (float x = spacing; x < bounds_.x - spacing; x += 2 * spacing)
    {
        for (float y = spacing; y < bounds_.y - spacing; y += 2 * spacing)
            startPositions_.emplace_back(x, y);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(startPositions_.begin(), startPositions_.end(), g);
}

void World::removeDestroyedDiscs()
{
    // TODO rename this function, have it iterate all discs and call various methods on each discs etc.
    currentKineticEnergy_ = 0.f;
    int currentIndex = 0;
    for (auto iter = discs_.begin(); iter != discs_.end();)
    {
        if (iter->isMarkedDestroyed())
        {
            iter = discs_.erase(iter);
            destroyedDiscsIndices_.push_back(currentIndex);
        }
        // TODO also find changed discs here, no need to iterate twice
        else
        {
            currentKineticEnergy_ += iter->getKineticEnergy();
            ++iter;
        }

        ++currentIndex;
    }
}