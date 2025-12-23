#include "MathUtils.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <numeric>
#include <ostream>
#include <random>

namespace cell::mathutils
{

std::vector<Vector2d> calculateGrid(double width, double height, double edgeLength)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::vector<Vector2d> gridPoints;
    gridPoints.reserve(static_cast<std::size_t>((static_cast<double>(width) / edgeLength) *
                                                (static_cast<double>(height) / edgeLength)));
    double spacing = edgeLength + 1;

    for (int i = 0; i < static_cast<int>(width / spacing); ++i)
    {
        for (int j = 0; j < static_cast<int>(height / spacing); ++j)
            gridPoints.emplace_back(spacing * static_cast<double>(i + 1), spacing * static_cast<double>(j + 1));
    }

    std::shuffle(gridPoints.begin(), gridPoints.end(), gen);

    return gridPoints;
}

} // namespace cell::mathutils