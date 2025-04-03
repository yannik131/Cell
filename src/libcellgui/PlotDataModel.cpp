#include "PlotDataModel.hpp"
#include "GlobalSettings.hpp"

PlotDataModel::PlotDataModel(QObject* parent)
    : QObject(parent)
{
    frameDTOs_.reserve(1000);
}

void PlotDataModel::setCurrentPlotCategory(const PlotCategory& plotCategory)
{
    if (plotCategory == currentPlotCategory_)
        return;

    currentPlotCategory_ = plotCategory;
    emitPlotData();
}

void PlotDataModel::receiveFrameDTO(const FrameDTO& frameDTO)
{
    frameDTOs_.push_back(frameDTO);

    elapsedWorldTimeSinceLastPlot_ += sf::microseconds(frameDTO.simulationTimeStepUs);
    if (elapsedWorldTimeSinceLastPlot_ >= GlobalSettings::getSettings().plotTimeInterval_)
    {
        emitPlotData();
        elapsedWorldTimeSinceLastPlot_ -= GlobalSettings::getSettings().plotTimeInterval_;
    }
}

void PlotDataModel::emitPlotData()
{
    PlotData plotData{.currentPlotCategory_ = currentPlotCategory_};
    sf::Time timeInDataPoint = sf::Time::Zero;

    if (currentPlotCategory_ == PlotCategory::TotalCollisionCount)
    {
        plotData.collisionCounts_.push_back(0);
        for (const auto& frameDTO : frameDTOs_)
        {
            timeInDataPoint += sf::microseconds(frameDTO.simulationTimeStepUs);
            if (timeInDataPoint >= GlobalSettings::getSettings().plotTimeInterval_)
            {
                timeInDataPoint -= GlobalSettings::getSettings().plotTimeInterval_;
                plotData.collisionCounts_.push_back(0);
            }
            plotData.collisionCounts_.back() += frameDTO.collisionCount_;
        }

        plotData.collisionCounts_.pop_back();
    }

    emit PlotDataModel::plotData(plotData);
}
