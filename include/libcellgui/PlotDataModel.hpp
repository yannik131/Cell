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

    void setCurrentPlotCategory(const PlotCategory& plotCategory);

public slots:
    void receiveFrameDTO(const FrameDTO& frameDTO);

signals:
    void plotData(const PlotData& plotData);

private:
    void emitPlotData();

private:
    std::vector<FrameDTO> frameDTOs_;
    PlotCategory currentPlotCategory_ = PlotCategory::TotalCollisionCount;
    sf::Time elapsedWorldTimeSinceLastPlot_ = sf::Time::Zero;
};

#endif /* PLOTDATAMODEL_HPP */
