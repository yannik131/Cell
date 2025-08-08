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