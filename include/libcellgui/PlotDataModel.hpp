#ifndef PLOTDATAMODEL_HPP
#define PLOTDATAMODEL_HPP

#include "FrameDTO.hpp"
#include "PlotCategories.hpp"
#include "PlotData.hpp"

#include <QObject>

#include <SFML/System/Time.hpp>

class PlotDataModel : public QObject
{
    Q_OBJECT
public:
    PlotDataModel(QObject* parent = nullptr);

    void setCurrentPlotCategory(const QString& plotCategoryName);

public slots:
    void receiveFrameDTO(const FrameDTO& frameDTO);
    void receiveSelectedDiscTypeNames(const QStringList& selectedDiscTypeNames);

signals:
    void plotData(const PlotData& plotData);

private:
    void emitPlotData();
    void fillCollisionCounts(PlotData& plotData);
    void fillDiscTypeCounts(PlotData& plotData);

private:
    std::vector<FrameDTO> frameDTOs_;
    std::vector<DiscType> activeDiscTypes_;
    PlotCategory currentPlotCategory_;
    sf::Time elapsedWorldTimeSinceLastPlot_ = sf::Time::Zero;
    const sf::Time& plotInterval_;
};

#endif /* PLOTDATAMODEL_HPP */
