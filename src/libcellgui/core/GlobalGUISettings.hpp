#ifndef GLOBALGUISETTINGS_HPP
#define GLOBALGUISETTINGS_HPP

#include "GUISettings.hpp"

#include <QObject>

class GlobalGUISettings : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Getter for the settings singleton
     */
    static GlobalGUISettings& get();

    /**
     * @returns Read-only copy of the currently set GUI settings
     */
    static const GUISettings& getGUISettings();

    /**
     * @brief Sets the refresh rate for the simulation widget
     */
    void setGuiFPS(int guiFPS);

    /**
     * @brief Sets the refresh rate of the simulation statistics plot, also changing the interval for averaging
     */
    void setPlotTimeInterval(const sf::Time& plotTimeInterval);

    /**
     * @brief Sets the type of the current simulation statistic plot
     */
    void setCurrentPlotCategory(const PlotCategory& plotCategory);

    /**
     * @brief Enables only the plot types contained in the given list for the analysis plot
     */
    void setDiscTypesPlotMap(const QStringList& selectedDiscTypeNames);

    /**
     * @brief Sets whether or not the sum for all disc types of the currently selected plot type should be plotted
     */
    void setPlotSum(bool value);

    /**
     * @brief Maintains the plot status for the previous disc types, enable plotting for all new ones and remove disc
     * types from the plot map that are no longer in the current disc type distribution
     */
    void updateDiscTypesPlotMap();

signals:
    void replotRequired();
    void plotDataMapUpdated();

private:
    GlobalGUISettings();

private:
    GUISettings guiSettings_;
};

#endif /* GLOBALGUISETTINGS_HPP */
