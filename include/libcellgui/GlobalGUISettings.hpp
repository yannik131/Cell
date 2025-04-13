#ifndef GLOBALGUISETTINGS_HPP
#define GLOBALGUISETTINGS_HPP

#include "GUISettings.hpp"

class GlobalGUISettings
{
public:
    static GlobalGUISettings& get();

    static const GUISettings& getGUISettings();

private:
    GlobalGUISettings();

    void loadDefaultDiscTypesPlotMap();

    void setGuiFPS(int guiFPS);

    void setPlotTimeInterval(const sf::Time& plotTimeInterval);

    void setCurrentPlotCategory(const PlotCategory& plotCategory);

    void setDiscTypesPlotMap(const QMap<DiscType, bool>& discTypesPlotMap);

private:
    GUISettings guiSettings_;
};

#endif /* GLOBALGUISETTINGS_HPP */
