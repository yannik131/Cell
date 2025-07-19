#ifndef PHYSICALOBJECTNANOFLANNADAPTER_HPP
#define PHYSICALOBJECTNANOFLANNADAPTER_HPP

#include "PhysicalObject.hpp"

#include <nanoflann.hpp>

namespace cell
{

/**
 * @brief Wrapper around a vector<PhysicalObject> for nanoflann
 */
template <typename T>
    requires std::is_base_of_v<PhysicalObject, T>
struct PhysicalObjectNanoflannAdapter
{
    const std::vector<T>& elements;

    explicit PhysicalObjectNanoflannAdapter(const std::vector<T>& elems)
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

#endif /* PHYSICALOBJECTNANOFLANNADAPTER_HPP */
