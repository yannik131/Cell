#ifndef DD74492E_28F5_4688_960F_C9E90C40C167_HPP
#define DD74492E_28F5_4688_960F_C9E90C40C167_HPP

#include "MembraneType.hpp"
#include "Types.hpp"
#include "Vector2d.hpp"

namespace cell
{

class Membrane
{
public:
    explicit Membrane(const MembraneTypeID& membraneTypeID);

    const sf::Vector2d& getPosition() const;
    void setPosition(const sf::Vector2d& position);

private:
    MembraneTypeID membraneTypeID_;
    sf::Vector2d position_;
};

} // namespace cell

#endif /* DD74492E_28F5_4688_960F_C9E90C40C167_HPP */
