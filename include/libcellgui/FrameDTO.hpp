#ifndef FRAMEDTO_HPP
#define FRAMEDTO_HPP

#include "Disc.hpp"

#include <QtCore/QMetaType>
#include <SFML/System/Vector2.hpp>

#include <vector>

struct FrameDTO
{
    std::vector<Disc> discs_;
    int collisionCount_;
    long long simulationTimeStepUs;
};

Q_DECLARE_METATYPE(FrameDTO);

#endif /* FRAMEDTO_HPP */
