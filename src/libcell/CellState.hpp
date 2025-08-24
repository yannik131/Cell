#ifndef state_HPP
#define state_HPP

#include "DiscType.hpp"
#include "Types.hpp"

namespace cell
{

class Disc;

class CellState
{
public:
    CellState(DiscTypeResolver discTypeResolver);

    void setNumberOfDiscs(int numberOfDiscs);
    void setCellWidth(int cellWidth);
    void setCellHeight(int cellHeight);
    void setDiscTypeDistribution(const DiscTypeMap<int> discTypeDistribution);

    /**
     * @brief Initializes the starting positions and creates the discs in the cell according to the distribution in the
     * settings
     * @note Has to be called at least once before calling `update()`
     */
    void randomizeUsingDiscTypeDistribution();

private:
    void build(std::vector<sf::Vector2d> discPositions);

private:
    /**
     * @brief Total number of discs in the simulation
     */
    int numberOfDiscs_ = 100;

    /**
     * @brief Width of the cell, arbitrary unit
     */
    int cellWidth_ = 1000;

    /**
     * @brief Height of the cell, arbitrary unit
     */
    int cellHeight_ = 1000;

    std::vector<Disc> discs_;

    /**
     * @brief Stores the percentage of each disc type for the start of the simulation
     */
    DiscTypeMap<int> discTypeDistribution_;

    DiscTypeResolver discTypeResolver_;

    // TODO remove, the simulation doesn't care about this
    double initialKineticEnergy_ = 0;
    double currentKineticEnergy_ = 0;

    friend class Cell;
};

} // namespace cell

#endif /* state_HPP */
