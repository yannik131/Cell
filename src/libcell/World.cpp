#include "World.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "NanoflannAdapter.hpp"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <execution>
#include <map>
#include <random>
#include <set>

World::World()
{
}

void World::update(const sf::Time& dt)
{
    for (auto& disc : discs_)
    {
        disc.position_ += disc.velocity_ * dt.asSeconds();
        const auto& collidingDiscs = MathUtils::findCollidingDiscs(discs_, maxRadius_);
        collisionCount_ += MathUtils::handleDiscCollisions(collidingDiscs, dt);
        MathUtils::handleWorldBoundCollision(disc, bounds_);
    }
}

int World::getAndResetCollisionCount()
{
    int tmp = collisionCount_;
    collisionCount_ = 0;

    return tmp;
}

const std::vector<Disc>& World::discs() const
{
    return discs_;
}

void World::reinitialize()
{
    const auto& discTypeDistribution = GlobalSettings::getSettings().discTypeDistribution_;

    maxRadius_ = std::max_element(discTypeDistribution.begin(), discTypeDistribution.end(),
                                  [](const auto& a, const auto& b) { return a.first.radius_ < b.first.radius_; })
                     ->first.radius_;

    discs_.clear();
    startPositions_.clear();

    initializeStartPositions();
    buildScene();
}

void World::setBounds(const sf::Vector2f& bounds)
{
    if (bounds.x <= 0 || bounds.y <= 0)
        throw std::runtime_error("Bounds must be > 0");

    bounds_ = bounds;
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

    if (settings.numberOfDiscs_ > startPositions_.size())
        LOG(WARNING) << "According to the settings, " << std::to_string(settings.numberOfDiscs_)
                     << " discs should be created, but the render window can only fit "
                     << std::to_string(startPositions_.size()) << ". "
                     << std::to_string(settings.numberOfDiscs_ - startPositions_.size())
                     << " discs will not be created.";

    // We need the accumulated percentages sorted in ascending order for the random number approach to work
    std::vector<std::pair<DiscType, int>> discTypes;
    for (const auto& pair : settings.discTypeDistribution_)
        discTypes.push_back(
            std::make_pair(pair.first, pair.second + (discTypes.empty() ? 0 : discTypes.back().second)));

    std::sort(discTypes.begin(), discTypes.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

    for (int i = 0; i < settings.numberOfDiscs_ && !startPositions_.empty(); ++i)
    {
        int randomNumber = distribution(gen);

        for (const auto& [discType, percentage] : discTypes)
        {
            if (randomNumber < percentage)
            {
                counts[discType.radius_]++;
                Disc newDisc(discType);
                newDisc.position_ = startPositions_.back();
                newDisc.velocity_ = sf::Vector2f(velocityDistribution(gen), velocityDistribution(gen));

                discs_.push_back(newDisc);
                startPositions_.pop_back();

                break;
            }
        }
    }

    VLOG(1) << "Radius distribution";
    for (const auto& [radius, count] : counts)
    {
        VLOG(1) << radius << ": " << count << "/" << settings.numberOfDiscs_ << " ("
                << count / static_cast<float>(settings.numberOfDiscs_) * 100 << "%)\n";
    }
}

void World::initializeStartPositions()
{
    if (bounds_.x == 0 || bounds_.y == 0)
        throw std::runtime_error("Can't initialize world: Bounds not set");

    startPositions_.reserve((bounds_.x / maxRadius_) * (bounds_.y / maxRadius_));

    float spacing = maxRadius_ + 1;

    for (float x = spacing; x < bounds_.x - spacing; x += 2 * spacing)
    {
        for (float y = spacing; y < bounds_.y - spacing; y += 2 * spacing)
            startPositions_.push_back(sf::Vector2f(x, y));
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(startPositions_.begin(), startPositions_.end(), g);
}
