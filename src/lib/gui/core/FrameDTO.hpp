#ifndef FRAMEDTO_HPP
#define FRAMEDTO_HPP

#include "cell/Disc.hpp"

#include <QtCore/QMetaType>
#include <SFML/System/Vector2.hpp>

#include <vector>

/**
 * @brief POD used to transmit information on all discs in the simulation to the widget drawing it and the plot model
 */
struct FrameDTO
{
    std::vector<cell::Disc> discs_;
    cell::DiscTypeMap<int> collisionCounts_;
    long long elapsedSimulationTimeUs{};
};

Q_DECLARE_METATYPE(FrameDTO);

#endif /* FRAMEDTO_HPP */
