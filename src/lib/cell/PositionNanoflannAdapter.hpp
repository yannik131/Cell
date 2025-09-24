#ifndef FD7966C7_C362_41FF_A22A_D6461037B89B_HPP
#define FD7966C7_C362_41FF_A22A_D6461037B89B_HPP

#include "Vector2d.hpp"

#include <nanoflann.hpp>

namespace cell
{

template <typename T>
concept HasGetPosition = requires(T t) {
    { t.getPosition() } -> std::same_as<const sf::Vector2d&>;
};

/**
 * @brief Wrapper around objects providing a `getPosition()` method for nanoflann
 */
template <typename T>
    requires HasGetPosition<T>
struct PositionNanoflannAdapter
{
    const std::vector<T>& elements;

    explicit PositionNanoflannAdapter(const std::vector<T>& elems)
        : elements(elems)
    {
    }

    inline size_t kdtree_get_point_count() const
    {
        return elements.size();
    }

    inline double kdtree_get_pt(const size_t idx, int dim) const
    {
        if (dim == 0)
            return elements[idx].getPosition().x;
        else
            return elements[idx].getPosition().y;
    }

    template <class BBOX> bool kdtree_get_bbox(BBOX&) const
    {
        return false;
    }
};

} // namespace cell

#endif /* FD7966C7_C362_41FF_A22A_D6461037B89B_HPP */
