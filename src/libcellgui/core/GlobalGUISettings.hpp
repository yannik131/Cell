#ifndef GLOBALGUISETTINGS_HPP
#define GLOBALGUISETTINGS_HPP

#include "GUISettings.hpp"

#include <QObject>

// TODO Use separate namespaces for GUI and backend to avoid crappy names and confusion

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

signals:
    void replotRequired();

private:
    GlobalGUISettings();

    void loadDefaultDiscTypesPlotMap();

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
     * @brief
     */
    void setDiscTypesPlotMap(const DiscType::map<bool>& discTypesPlotMap);
    void setDiscTypesPlotMap(const QStringList& selectedDiscTypeNames);

    void setPlotSum(bool value);

    void updateDiscTypesPlotMap();

private:
    GUISettings guiSettings_;

    friend class SimulationControlWidget;
    friend class PlotControlWidget;
    friend class PlotDataSelectionDialog;
};

#endif /* GLOBALGUISETTINGS_HPP */
