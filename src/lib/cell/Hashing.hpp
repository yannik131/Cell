#ifndef HASHING_HPP
#define HASHING_HPP

#include <string>
#include <type_traits>
#include <utility>

namespace cell
{

// https://stackoverflow.com/a/2595226
template <class T> inline void hashCombine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/**
 * @returns a well-mixed combined hash for 2 values
 */
template <typename T1, typename T2> std::size_t calculateHash(const T1& a, const T2& b)
{
    std::size_t seed = 0;
    hashCombine(seed, a);
    hashCombine(seed, b);

    return seed;
}

/**
 * @brief A generic pair hasher using std::hash and the boost method for mixing 2 hash values
 */
struct PairHasher
{
    template <typename T1, typename T2> std::size_t operator()(const std::pair<T1, T2>& pair) const
    {
        return calculateHash(pair.first, pair.second);
    }
};

} // namespace cell

#endif /* HASHING_HPP */
