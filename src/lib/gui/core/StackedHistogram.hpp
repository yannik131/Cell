#ifndef ED08D714_0D27_46A0_8E10_60B77D36F541_HPP
#define ED08D714_0D27_46A0_8E10_60B77D36F541_HPP

#include "cell/ExceptionWithLocation.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

class StackedHistogram
{
public:
    template <typename T, typename GetValue> void initializeBins(std::vector<T>& elements, GetValue getValue);
    void increaseCount(const std::string& key, double value);

private:
    template <typename T, typename GetValue>
    double quantile(const std::vector<T>& v, double q, GetValue getValue) const;

private:
    double binWidth_ = 0;
    double leftX_ = 0;
    int binCount_ = 0;
    std::unordered_map<std::string, std::unordered_map<int, int>> histogramsByKeys_;
};

template <typename T, typename GetValue>
inline void StackedHistogram::initializeBins(std::vector<T>& elements, GetValue getValue)
{
    if (binCount_ > 0)
        throw ExceptionWithLocation("Histogram already initialized");

    std::sort(elements.begin(), elements.end(),
              [&](const cell::Disc& lhs, const cell::Disc& rhs) { return getValue(lhs) < getValue(rhs); });

    const double Q25 = quantile(elements, 0.25, getValue);
    const double Q75 = quantile(elements, 0.75, getValue);
    const double IQR = Q75 - Q25;
    binWidth_ = 2 * IQR / std::cbrt(static_cast<double>(elements.size()));
    binCount_ = binWidth_ > 0 ? std::max(1, static_cast<int>(std::ceil(
                                                (getValue(elements.back()) - getValue(elements.front())) / binWidth_)))
                              : 1;
    leftX_ = getValue(elements.front());
}

inline void StackedHistogram::increaseCount(const std::string& key, double value)
{
    int bin = binWidth_ > 0 ? std::min(binCount_ - 1, static_cast<int>(std::floor((value - leftX_) / binWidth_))) : 0;
    ++histogramsByKeys_[key][bin];
}

template <typename T, typename GetValue>
inline double StackedHistogram::quantile(const std::vector<T>& v, double q, GetValue getValue)
{
    if (v.empty())
        throw ExceptionWithLocation("Empty vector passed");

    if (q < 0 || q > 1)
        throw ExceptionWithLocation("q must be in [0, 1]");

    double pos = q * static_cast<double>(v.size() - 1);
    std::size_t i = static_cast<std::size_t>(std::floor(pos));
    double epsilon = pos - i;

    if (i + 1 < v.size())
        return getValue(v[i]) * (1.0 - epsilon) + getValue(v[i + 1]) * epsilon;

    return getValue(v[i]);
}

#endif /* ED08D714_0D27_46A0_8E10_60B77D36F541_HPP */