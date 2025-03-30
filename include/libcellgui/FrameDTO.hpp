#ifndef FRAMEDTO_HPP
#define FRAMEDTO_HPP

#include "Disc.hpp"

#include <QtCore/QMetaType>
#include <SFML/System/Vector2.hpp>

#include <vector>

struct GUIDisc
{
    sf::Vector2f position_;
};

struct FrameDTO
{
    std::vector<GUIDisc> discs_;
};

struct UpdateDTO
{
    std::vector<int> destroyedDiscIndices_;
    std::vector<std::pair<int, DiscType>> changedDiscIndices_;
    std::vector<Disc> newDiscs_;
};

Q_DECLARE_METATYPE(FrameDTO);
Q_DECLARE_METATYPE(UpdateDTO);

#endif /* FRAMEDTO_HPP */
