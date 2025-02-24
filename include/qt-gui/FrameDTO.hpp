#ifndef FRAMEDTO_HPP
#define FRAMEDTO_HPP

#include "Disc.hpp"

#include <SFML/System/Vector2.hpp>
#include <QtCore/QMetaType>

#include <vector>

struct GUIDisc 
{
    GUIDisc(int index, const sf::Vector2f& position);

    int index_;
    sf::Vector2f position_;
};

struct FrameDTO
{
    std::vector<GUIDisc> discs_;
};

Q_DECLARE_METATYPE(FrameDTO);

#endif /* FRAMEDTO_HPP */
