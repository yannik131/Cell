#ifndef FRAMEDTO_HPP
#define FRAMEDTO_HPP

#include "Disc.hpp"

#include <QtCore/QMetaType>
#include <SFML/System/Vector2.hpp>

#include <map>
#include <vector>

struct FrameDTO
{
    std::vector<Disc> discs_;
    std::map<DiscType, int> collisionCounts_;
    long long simulationTimeStepUs;
};

Q_DECLARE_METATYPE(FrameDTO);

#endif /* FRAMEDTO_HPP */
