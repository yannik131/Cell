#ifndef GLOBALGUISETTINGS_HPP
#define GLOBALGUISETTINGS_HPP

#include "GUISettings.hpp"

#include <QObject>

// TODO Use separate namespaces for GUI and backend to avoid crappy names and confusion

class GlobalGUISettings : public QObject
{
    Q_OBJECT
public:
    static GlobalGUISettings& get();

    static const GUISettings& getGUISettings();

signals:
    void replotRequired();

private:
    GlobalGUISettings();

    void loadDefaultDiscTypesPlotMap();

    void setGuiFPS(int guiFPS);

    void setPlotTimeInterval(const sf::Time& plotTimeInterval);

    void setCurrentPlotCategory(const PlotCategory& plotCategory);

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
