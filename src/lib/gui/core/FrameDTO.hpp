#ifndef B8E204A6_FA69_4C96_A2FA_D56A9466EC54_HPP
#define B8E204A6_FA69_4C96_A2FA_D56A9466EC54_HPP

#include "cell/Disc.hpp"

#include <QtCore/QMetaType>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <chrono>
#include <vector>

namespace ch = std::chrono;

/**
 * @brief POD used to transmit information on all discs in the simulation to the widget drawing it and the plot model
 */
struct FrameDTO
{
    std::vector<cell::Disc> discs_;
    std::vector<sf::CircleShape> membranes_;
    cell::DiscTypeMap<int> collisionCounts_;
    ch::duration<double> elapsedSimulationTime_;
};

Q_DECLARE_METATYPE(FrameDTO);

#endif /* B8E204A6_FA69_4C96_A2FA_D56A9466EC54_HPP */
