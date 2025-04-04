#include "PlotDataModel.hpp"
#include "GlobalSettings.hpp"

#include <QDebug>

PlotDataModel::PlotDataModel(QObject* parent)
    : QObject(parent)
    , currentPlotCategory_(SupportedPlotCategories.first())
    , plotInterval_(GlobalSettings::getSettings().plotTimeInterval_)
{
    frameDTOs_.reserve(1000);
    currentPlotCategory_ = SupportedPlotCategories.first();
    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
        activeDiscTypes_.push_back(discType);
}

void PlotDataModel::setCurrentPlotCategory(const QString& plotCategoryName)
{
    const auto& plotCategory = NamePlotCategoryMapping[plotCategoryName];
    if (plotCategory == currentPlotCategory_)
        return;

    currentPlotCategory_ = plotCategory;
    emitPlotData();
}

void PlotDataModel::receiveSelectedDiscTypeNames(const QStringList& selectedDiscTypeNames)
{
    activeDiscTypes_.clear();
    for (const auto& selectedDiscTypeName : selectedDiscTypeNames)
        activeDiscTypes_.push_back(GlobalSettings::getDiscTypeByName(selectedDiscTypeName.toStdString()));

    emitPlotData();
}

void PlotDataModel::receiveFrameDTO(const FrameDTO& frameDTO)
{
    frameDTOs_.push_back(frameDTO);

    elapsedWorldTimeSinceLastPlot_ += sf::microseconds(frameDTO.simulationTimeStepUs);
    if (elapsedWorldTimeSinceLastPlot_ >= plotInterval_)
    {
        emitPlotData();
        elapsedWorldTimeSinceLastPlot_ -= plotInterval_;
    }
}

void PlotDataModel::emitPlotData()
{
    PlotData plotData{.currentPlotCategory_ = currentPlotCategory_};

    switch (currentPlotCategory_)
    {
    case PlotCategory::TotalCollisionCount: fillCollisionCounts(plotData); break;
    case PlotCategory::TypeCounts: fillDiscTypeCounts(plotData); break;
    }

    emit PlotDataModel::plotData(plotData);
}

void PlotDataModel::fillCollisionCounts(PlotData& plotData)
{
    plotData.collisionCounts_.push_back(0);
    sf::Time timeInDataPoint = sf::Time::Zero;

    for (const auto& frameDTO : frameDTOs_)
    {
        plotData.collisionCounts_.back() += frameDTO.collisionCount_;

        timeInDataPoint += sf::microseconds(frameDTO.simulationTimeStepUs);
        if (timeInDataPoint >= plotInterval_)
        {
            timeInDataPoint -= plotInterval_;
            plotData.collisionCounts_.push_back(0);
        }
    }

    plotData.collisionCounts_.pop_back();
}

void PlotDataModel::fillDiscTypeCounts(PlotData& plotData)
{
    sf::Time timeInDataPoint = sf::Time::Zero;
    int framesInDataPoint = 0;
    plotData.discTypeCounts_.push_back({});

    QMap<DiscType, bool> discTypeEnabled;
    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
    {
        auto iter = std::find(activeDiscTypes_.begin(), activeDiscTypes_.end(), discType);
        discTypeEnabled[discType] = iter != activeDiscTypes_.end();
    }

    for (const auto& frameDTO : frameDTOs_)
    {
        for (const auto& disc : frameDTO.discs_)
        {
            if (!discTypeEnabled[disc.getType()])
                continue;

            ++plotData.discTypeCounts_.back()[disc.getType()];
        }

        ++framesInDataPoint;
        timeInDataPoint += sf::microseconds(frameDTO.simulationTimeStepUs);

        if (timeInDataPoint >= plotInterval_)
        {
            timeInDataPoint -= plotInterval_;
            for (const auto& discType : activeDiscTypes_)
                plotData.discTypeCounts_.back()[discType] /= framesInDataPoint;

            plotData.discTypeCounts_.push_back({});
            framesInDataPoint = 0;
        }
    }

    plotData.discTypeCounts_.pop_back();
}
