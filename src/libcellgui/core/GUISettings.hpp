#ifndef GUISETTINGS_HPP
#define GUISETTINGS_HPP

#include "DiscType.hpp"
#include "PlotCategories.hpp"

#include <SFML/System/Time.hpp>

/**
 * @deprecated https://github.com/yannik131/Cell/issues/24
 */
struct GUISettings
{
    /**
     * @brief How many times a second the simulation in the SimulationWidget is redrawn
     */
    int guiFPS_ = 120;

    /**
     * @brief If enabled, only plot the total sum of all values
     */
    bool plotSum_ = false;

    /**
     * @brief How long to wait between plots
     */
    sf::Time plotTimeInterval_ = sf::milliseconds(1000);

    /**
     * @brief Type of plot in the PlotWidget
     */
    PlotCategory currentPlotCategory_ = PlotCategory::TypeCounts;

    /**
     * @brief Maps all DiscTypes to whether or not they're selected for the current plot
     */
    DiscType::map<bool> discTypesPlotMap_;
};

namespace GUISettingsLimits
{
const int MinGuiFPS = 0;
const int MaxGuiFPS = 200;

const sf::Time MinPlotTimeInterval = sf::milliseconds(100);
const sf::Time MaxPlotTimeInterval = sf::milliseconds(10000);
} // namespace GUISettingsLimits

#endif /* GUISETTINGS_HPP */
