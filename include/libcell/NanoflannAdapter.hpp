#ifndef NANOFLANNADAPTER_H
#define NANOFLANNADAPTER_H

#include "Particle.hpp"

#include <nanoflann.hpp>

struct NanoflannAdapter
{
    const std::vector<Particle>& elements;

    NanoflannAdapter(const std::vector<Particle>& elems)
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


#endif /* NANOFLANNADAPTER_H */
