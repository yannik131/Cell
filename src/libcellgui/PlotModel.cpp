#include "GlobalSettings.hpp"
#include "PlotModel.hpp"

#include <QDebug>

Frame averageFrame(const QVector<Frame>& frames)
{
    Frame average;
    for (const auto& frame : frames)
    {
        average.collisionCount_ += frame.collisionCount_;
        average.totalKineticEnergy_ += frame.totalKineticEnergy_;
        average.totalMomentum_ += frame.totalMomentum_;
        average.discTypeCount_ += frame.discTypeCount_;
    }

    average.collisionCount_ /= frames.size();
    average.totalKineticEnergy_ /= frames.size();
    average.totalMomentum_ /= frames.size();
    average.discTypeCount_ /= frame.size();

    return average;
}

void removeInactiveDiscTypesData(Frame& frame, const QMap<DiscType, bool> activeDiscTypesMap)
{
    for (auto iter = activeDiscTypesMap.begin(); iter != activeDiscTypesMap.end(); ++iter)
    {
        if (!iter.value())
        {
            frame.totalKineticEnergy_.remove(iter.key());
            frame.totalMomentum_.remove(iter.key());
            frame.discTypeCount_.remove(iter.key());
        }
    }
}

PlotModel::PlotModel(QObject* parent)
    : QObject(parent)
    , currentPlotCategory_(SupportedPlotCategories.first())
    , plotInterval_(GlobalSettings::getSettings().plotTimeInterval_)
{
    plotDataPoints_.reserve(1000);
    currentPlotCategory_ = SupportedPlotCategories.first();
    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
        activeDiscTypes_.push_back(discType);
}

void PlotModel::setCurrentPlotCategory(const QString& plotCategoryName)
{
    const auto& plotCategory = NamePlotCategoryMapping[plotCategoryName];
    if (plotCategory == currentPlotCategory_)
        return;

    currentPlotCategory_ = plotCategory;
    emitFullPlot();
}

void PlotModel::receiveSelectedDiscTypeNames(const QStringList& selectedDiscTypeNames)
{
    activeDiscTypesMap_.clear();

    QVector<DiscType> activeDiscTypes;
    for (const auto& selectedDiscTypeName : selectedDiscTypeNames)
        activeDiscTypes.push_back(GlobalSettings::getDiscTypeByName(selectedDiscTypeName.toStdString()));

    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
    {
        auto iter = std::find(activeDiscTypes.begin(), activeDiscTypes.end(), discType);
        activeDiscTypesMap_[discType] = iter != activeDiscTypes.end();
    }

    emitFullPlot();
}

void PlotModel::plotIntervalChanged()
{
    emitFullPlot();
}

void PlotModel::receiveFrameDTO(const FrameDTO& frameDTO)
{
    Frame frame;
    frame.collisionCount_ = frameDTO.collisionCount_;
    frame.elapsedTimeUs_ = frameDTO.simulationTimeStepUs;

    for (const auto& disc : frameDTO.discs_)
    {
        ++frame.discTypeCount_[disc.getType()];
        frame.totalKineticEnergy_[disc.getType()] += disc.getKineticEnergy();
        frame.totalMomentum_[disc.getType()] += disc.getAbsoluteMomentum();
    }

    framesToAverage_.push_back(frame);

    elapsedWorldTimeSinceLastPlot_ += sf::microseconds(frameDTO.simulationTimeStepUs);
    if (elapsedWorldTimeSinceLastPlot_ >= plotInterval_)
    {
        const auto& averagedFrame = averageFrame(framesToAverage_);
        frames_.append(std::move(framesToAverage_));
        framesToAverage_.clear();
        elapsedWorldTimeSinceLastPlot_ -= plotInterval_;

        emitPlotDataPoint(averagedFrame, currentPlotCategory_);
    }
}

void PlotModel::emitPlotDataPoint(Frame averagedFrame)
{
    removeInactiveDiscTypesData(averagedFrame);

    emit plotDataPoint(averagedFrame, currentPlotCategory_);
}

void PlotModel::emitFullPlot()
{
    QVector<Frame> fullPlotData;
    QVector<Frame> framesToAverage;
    sf::Time elapsedTime = sf::Time::Zero;

    for (const auto& frame : frames_)
    {
        framesToAverage.push_back(frame);

        elapsedTime += sf::microseconds(frame.elapsedTimeUs_);
        if (elapsedTime >= plotInterval_)
        {
            Frame averagedFrame = averageFrame(framesToAverage_);
            removeInactiveDiscTypesData(averagedFrame);
            fullPlotData.append(averagedFrame);
            framesToAverage_.clear();
            elapsedTime -= plotInterval_;
        }
    }

    emit fullPlot(fullPlotData, currentPlotCategory_);
}