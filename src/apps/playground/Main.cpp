#include <boost/histogram.hpp>
#include <boost/histogram/indexed.hpp>
#include <iostream>
#include <random>
#include <vector>

namespace bh = boost::histogram;

// helper: dst += src
template <class H> void add_assign(H& dst, const H& src)
{
    for (auto&& cell : bh::indexed(dst))
    {
        dst.at(cell.indices()) += src.at(cell.indices());
    }
}

int main()
{
    constexpr int totalSlices = 100;      // 100 × 100ms = 10s
    constexpr double sliceDuration = 0.1; // 100ms

    // Shared axes (identical for all slices)
    auto base = bh::make_histogram(bh::axis::integer<int>(0, 2, "series"),    // 2 object types
                                   bh::axis::regular<>(50, -10.0, 10.0, "vx") // fixed bins
    );

    // --- create and fill slices ---
    std::vector<decltype(base)> slices;
    slices.reserve(totalSlices);

    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<double> dist0(0.0, 2.0);
    std::normal_distribution<double> dist1(1.0, 3.0);

    for (int s = 0; s < totalSlices; ++s)
    {
        auto h = base;
        h.reset();

        // fill slice with random data
        for (int i = 0; i < 200; ++i)
        {
            h(0, dist0(rng)); // series 0
            h(1, dist1(rng)); // series 1
        }

        slices.push_back(std::move(h));
    }

    // --- user selects interval ---
    double interval = 2.0; // seconds

    int slicesPerWindow = static_cast<int>(interval / sliceDuration);
    int numWindows = totalSlices / slicesPerWindow;

    std::vector<decltype(base)> windows;
    windows.reserve(numWindows);

    for (int w = 0; w < numWindows; ++w)
    {
        auto sum = base;
        sum.reset();

        int start = w * slicesPerWindow;
        int end = start + slicesPerWindow;

        for (int i = start; i < end; ++i)
        {
            add_assign(sum, slices[i]);
        }

        windows.push_back(std::move(sum));
    }

    // demo output: print first bin of each window for series 0
    for (int w = 0; w < numWindows; ++w)
    {
        std::cout << "Window " << w << " series0 bin0 count = " << windows[w].at(0, 0) << "\n";
    }

    return 0;
}