#ifndef GLOBALGUISETTINGS_HPP
#define GLOBALGUISETTINGS_HPP

#include "GUISettings.hpp"

#include <QObject>

class GlobalGUISettings : public QObject
{
    Q_OBJECT
public:
    static GlobalGUISettings& get();

    static const GUISettings& getGUISettings();

signals:
    void plotResetRequired();

private:
    GlobalGUISettings();

    void loadDefaultDiscTypesPlotMap();

    void setGuiFPS(int guiFPS);

    void setPlotTimeInterval(const sf::Time& plotTimeInterval);

    void setCurrentPlotCategory(const PlotCategory& plotCategory);

    void setDiscTypesPlotMap(const QMap<DiscType, bool>& discTypesPlotMap);
    void setDiscTypesPlotMap(const QStringList& selectedDiscTypeNames);

    void updateDiscTypesPlotMap();

private:
    GUISettings guiSettings_;

    friend class SimulationControlWidget;
    friend class PlotControlWidget;
    friend class PlotDataSelectionDialog;
};

#endif /* GLOBALGUISETTINGS_HPP */
