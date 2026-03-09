#include "models/PlotModel.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "cell/MathUtils.hpp"
#include "core/Simulation.hpp"
#include "core/Utility.hpp"

#include <cmath>
#include <numeric>
#include <unordered_set>

namespace
{

void averageDataPoint(DataPoint& dataPoint, int length)
{
    double dt = dataPoint.elapsedTime_;

    // Collisions per second = All registered collisions / dt
    utility::divideValuesBy(dataPoint.collisionCounts_, dt);

    utility::divideValuesBy(dataPoint.totalKineticEnergyMap_, length);
    utility::divideValuesBy(dataPoint.totalMomentumMap_, length);
    utility::divideValuesBy(dataPoint.discTypeCountMap_, length);
    dataPoint.vxHistogram_ /= length;
}

} // namespace

PlotModel::PlotModel(QObject* parent, Simulation* simulation)
    : QObject(parent)
    , simulation_(simulation)
    , dataPointForStorage_(DataPoint(simulation->getSimulationConfig()))
    , dataPointForPlotting_(DataPoint(simulation->getSimulationConfig()))
{
    // With a simulation time step of 1ms, we get 1000 data points each second
    // With an averaging time of 100ms, we save 10 datapoints for 1 second
    // We'll reserve enough space for 5 minutes of plotting, 5*60*10
    dataPoints_.reserve(3000);

    // Keep histogram axes up to date
    connect(&simulation->getSimulationConfigUpdater(), &SimulationConfigUpdater::simulationResetRequired,
            [&]()
            {
                dataPointForStorage_ = DataPoint(simulation_->getSimulationConfig());
                dataPointForPlotting_ = DataPoint(simulation_->getSimulationConfig());
                initialVxHistogram_.reset();
            });
}

void PlotModel::setPlotCategory(PlotCategory plotCategory)
{
    static const std::unordered_map<PlotCategory, int> graphType{
        {PlotCategory::AbsoluteMomentum, 1},     {PlotCategory::CollisionCounts, 1},
        {PlotCategory::KineticEnergy, 1},        {PlotCategory::TypeCounts, 1},
        {PlotCategory::VelocityDistribution, 2}, {PlotCategory::VelocityHeatMap, 3}};

    const auto oldPlotCategory = plotCategory_;
    plotCategory_ = plotCategory;

    if (graphType.at(oldPlotCategory) != graphType.at(plotCategory))
        emitGraphs();

    emitWholePlot();
}

void PlotModel::setPlotTimeInterval(int valueMilliseconds)
{
    if (valueMilliseconds < 100)
        throw ExceptionWithLocation("The plot time interval must be at least 100ms");

    if (valueMilliseconds % 100 != 0)
        throw ExceptionWithLocation("The plot time interval must be a multiple of 100ms");

    plotTimeInterval_ = static_cast<double>(valueMilliseconds) / 1000.0;
    emitWholePlot();
}

void PlotModel::setPlotSum(bool value)
{
    plotSum_ = value;
    emitGraphs();
    emitWholePlot();
}

void PlotModel::reset()
{
    dataPoints_.clear();
    dataPointForStorage_.clear();
    dataPointForPlotting_.clear();
    initialVxHistogram_.reset();
    averagingCount_ = 0;

    updateActivePlotDiscTypes(simulation_->getSimulationConfig().discTypes);

    emitGraphs();
    emitWholePlot();
}

void PlotModel::setActivePlotDiscTypes(const std::vector<std::string>& activeDiscTypeNames)
{
    activePlotDiscTypes_.clear();
    for (const auto& name : activeDiscTypeNames)
        activePlotDiscTypes_[name] = true;

    emitGraphs();
    emitWholePlot();
}

const std::map<std::string, bool>& PlotModel::getActivePlotDiscTypesMap() const
{
    return activePlotDiscTypes_;
}

void PlotModel::captureInitialHistogram(const FrameDTO& frameDTO)
{
    const bool noSimulationDataCollectedYet =
        dataPoints_.empty() && averagingCount_ == 0 && dataPointForStorage_.elapsedTime_ == 0 &&
        dataPointForPlotting_.elapsedTime_ == 0 && !initialVxHistogram_.has_value();

    if (!noSimulationDataCollectedYet)
        return;

    DataPoint initialDataPoint = dataPointFromFrameDTO(frameDTO);
    initialVxHistogram_ = initialDataPoint.vxHistogram_;

    // Für das normale Histogramm soll beim Start sofort etwas sichtbar sein
    dataPointForPlotting_ = initialDataPoint;
}

void PlotModel::processFrame(const FrameDTO& frameDTO)
{
    // Elapsed time 0 means this DTO was only emitted for a redraw.
    // We still use that first redraw to initialize histogram/heatmap with the initial state.
    if (frameDTO.elapsedSimulationTimeUs == 0)
    {
        captureInitialHistogram(frameDTO);

        if (plotCategory_ == PlotCategory::VelocityDistribution || plotCategory_ == PlotCategory::VelocityHeatMap)
            emitWholePlot();

        return;
    }

    DataPoint dataPoint = dataPointFromFrameDTO(frameDTO);
    storeDataPoint(dataPoint);

    if (dataPointForPlotting_.elapsedTime_ >= plotTimeInterval_)
    {
        emitPlotPart();
        dataPointForPlotting_.clear();
    }
}

void PlotModel::emitWholePlot()
{
    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
    case PlotCategory::AbsoluteMomentum:
    case PlotCategory::CollisionCounts:
    case PlotCategory::KineticEnergy: emitLinePlot(); break;
    case PlotCategory::VelocityDistribution: emitHistogram(); break;
    case PlotCategory::VelocityHeatMap: emitHeatMap(); break;
    default: throw ExceptionWithLocation("Invalid plot category");
    }
}

void PlotModel::emitLinePlot()
{
    std::vector<std::unordered_map<std::string, double>> fullPlotData;
    DataPoint dataPointToAverage(simulation_->getSimulationConfig());
    int averagingCount = 0;
    double timeStep = simulation_->getSimulationConfig().simulationTimeStep;
    const int dataPointsPerStoredPoint = static_cast<int>(std::ceil(storageTime_ / timeStep));

    for (const auto& dataPoint : dataPoints_)
    {
        dataPointToAverage += dataPoint;
        ++averagingCount;

        if (dataPointToAverage.elapsedTime_ < plotTimeInterval_)
            continue;

        averageDataPoint(dataPointToAverage, averagingCount * dataPointsPerStoredPoint);
        const auto& activeMap = getActiveMap(dataPointToAverage);

        if (plotSum_)
        {
            auto sum = std::accumulate(activeMap.begin(), activeMap.end(), 0.0,
                                       [](double partialSum, const auto& pair) { return pair.second + partialSum; });
            fullPlotData.push_back({{"Sum", sum}});
        }
        else
            fullPlotData.push_back(getActiveMap(dataPointToAverage));

        dataPointToAverage.clear();
        averagingCount = 0;
    }

    emit createLinePlots(labels_, colors_);
    emit linePlotPoints(fullPlotData, plotTimeInterval_);
}

void PlotModel::emitHistogram()
{
    auto h = discardInactiveDiscTypes(dataPointForPlotting_.vxHistogram_);
    if (plotSum_)
        h = sumHistogramStacks(h);

    emit histogram(h);
}

Histogram PlotModel::makeActiveHeatMapHistogram(const Histogram& histogram) const
{
    auto filtered = discardInactiveDiscTypes(histogram);

    // Eine einzelne Heatmap kann nur einen Wert pro Zelle darstellen.
    // Deshalb werden alle aktiven Typen zu einer gemeinsamen Spalte zusammengefasst.
    return sumHistogramStacks(filtered);
}

HeatMapColumn PlotModel::makeHeatMapColumn(const Histogram& histogram) const
{
    const auto& regularAxis = histogram.axis<1>();
    HeatMapColumn column;
    column.reserve(static_cast<qsizetype>(regularAxis.size()));

    for (int i = 0; i < regularAxis.size(); ++i)
        column.push_back(histogram.at(0, i));

    return column;
}

HeatMapData PlotModel::buildWholeHeatMap() const
{
    HeatMapData columns;

    if (initialVxHistogram_.has_value())
        columns.push_back(makeHeatMapColumn(makeActiveHeatMapHistogram(*initialVxHistogram_)));

    DataPoint dataPointToAverage(simulation_->getSimulationConfig());
    int averagingCount = 0;
    double timeStep = simulation_->getSimulationConfig().simulationTimeStep;
    const int dataPointsPerStoredPoint = static_cast<int>(std::ceil(storageTime_ / timeStep));

    for (const auto& dataPoint : dataPoints_)
    {
        dataPointToAverage += dataPoint;
        ++averagingCount;

        if (dataPointToAverage.elapsedTime_ < plotTimeInterval_)
            continue;

        averageDataPoint(dataPointToAverage, averagingCount * dataPointsPerStoredPoint);
        columns.push_back(makeHeatMapColumn(makeActiveHeatMapHistogram(dataPointToAverage.vxHistogram_)));

        dataPointToAverage.clear();
        averagingCount = 0;
    }

    return columns;
}

void PlotModel::emitHeatMap()
{
    const auto& axis = dataPointForPlotting_.vxHistogram_.axis<1>();
    const int yCells = axis.size();

    HeatMapData columns = buildWholeHeatMap();

    const int xCells = std::max(1, static_cast<int>(columns.size()));
    const double xMax = std::max(plotTimeInterval_, static_cast<double>(columns.size()) * plotTimeInterval_);

    emit createHeatMap(0.0, xMax, axis.value(0), axis.value(axis.size()), xCells, yCells);
    emit heatMap(columns);
}

std::unordered_map<std::string, double> PlotModel::getActiveMap(const DataPoint& dataPoint)
{
    std::unordered_map<std::string, double> activeMap;

    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts: activeMap = dataPoint.discTypeCountMap_; break;
    case PlotCategory::CollisionCounts: activeMap = dataPoint.collisionCounts_; break;
    case PlotCategory::AbsoluteMomentum: activeMap = dataPoint.totalMomentumMap_; break;
    case PlotCategory::KineticEnergy: activeMap = dataPoint.totalKineticEnergyMap_; break;
    default: activeMap = {};
    }

    for (auto iter = activeMap.begin(); iter != activeMap.end();)
    {
        if (!activePlotDiscTypes_[iter->first])
            iter = activeMap.erase(iter);
        else
            ++iter;
    }

    return activeMap;
}

void PlotModel::storeDataPoint(const DataPoint& dataPoint)
{
    dataPointForStorage_ += dataPoint;

    if (dataPointForStorage_.elapsedTime_ >= storageTime_)
    {
        dataPoints_.push_back(dataPointForStorage_);
        dataPointForStorage_.clear();
    }

    dataPointForPlotting_ += dataPoint;
    ++averagingCount_;

    if (dataPointForPlotting_.elapsedTime_ >= plotTimeInterval_)
    {
        averageDataPoint(dataPointForPlotting_, averagingCount_);
        averagingCount_ = 0;
    }
}

void PlotModel::emitPlotPart()
{
    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
    case PlotCategory::AbsoluteMomentum:
    case PlotCategory::CollisionCounts:
    case PlotCategory::KineticEnergy: emitLinePlotPoints(); break;
    case PlotCategory::VelocityDistribution: emitHistogram(); break;
    case PlotCategory::VelocityHeatMap: emitHeatMapColumn(); break;
    default: throw ExceptionWithLocation("Invalid plot category");
    }
}

void PlotModel::emitLinePlotPoints()
{
    const auto& activeMap = getActiveMap(dataPointForPlotting_);

    if (plotSum_)
    {
        auto sum = std::accumulate(activeMap.begin(), activeMap.end(), 0.0,
                                   [&](double partialSum, const auto& pair) { return pair.second + partialSum; });
        emit linePlotPoint({{"Sum", sum}}, plotTimeInterval_, DoReplot{true});
    }
    else
        emit linePlotPoint(activeMap, plotTimeInterval_, DoReplot{true});
}

void PlotModel::emitHeatMapColumn()
{
    // Für die Heatmap regenerieren wir den kompletten Plot aus den gespeicherten Intervallen.
    // Das ist robust bei Plotintervall-Wechseln und beim Umschalten des Plottyps.
    emitHeatMap();
}

void PlotModel::emitGraphs()
{
    labels_.clear();
    colors_.clear();

    const auto& config = simulation_->getSimulationConfig();
    const auto& colorMap = simulation_->getSimulationConfigUpdater().getDiscTypeColorMap();

    if (plotSum_)
    {
        labels_.emplace_back("Sum");
        colors_.push_back(sf::Color::Black);
    }
    else
    {
        for (const auto& discType : config.discTypes)
        {
            if (!activePlotDiscTypes_[discType.name])
                continue;

            labels_.push_back(discType.name);
            colors_.push_back(colorMap.at(discType.name));
        }
    }

    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
    case PlotCategory::AbsoluteMomentum:
    case PlotCategory::CollisionCounts:
    case PlotCategory::KineticEnergy: emit createLinePlots(labels_, colors_); break;
    case PlotCategory::VelocityDistribution:
        emit createHistogram(labels_, colors_, dataPointForPlotting_.vxHistogram_);
        break;
    case PlotCategory::VelocityHeatMap:
    {
        const auto& axis = dataPointForPlotting_.vxHistogram_.axis<1>();
        emit createHeatMap(0.0, plotTimeInterval_, axis.value(0), axis.value(axis.size()), 1, axis.size());
        break;
    }
    default: throw ExceptionWithLocation("Invalid plot category");
    }
}

void PlotModel::updateActivePlotDiscTypes(const std::vector<cell::config::DiscType>& discTypes)
{
    std::unordered_set<std::string> discTypeNames;

    for (const auto& discType : discTypes)
    {
        if (!activePlotDiscTypes_.contains(discType.name))
            activePlotDiscTypes_[discType.name] = true;

        discTypeNames.insert(discType.name);
    }

    for (auto iter = activePlotDiscTypes_.begin(); iter != activePlotDiscTypes_.end();)
    {
        if (!discTypeNames.contains(iter->first))
            iter = activePlotDiscTypes_.erase(iter);
        else
            ++iter;
    }
}

Histogram PlotModel::sumHistogramStacks(const Histogram& histogram) const
{
    const auto& categoryAxis = histogram.axis<0>();
    const auto& regularAxis = histogram.axis<1>();
    Histogram sumHistogram = makeHistogramWithCategories(histogram, {"Sum"});

    for (int i = 0; i < regularAxis.size(); ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < categoryAxis.size(); ++j)
            sum += histogram.at(j, i);

        sumHistogram.at(0, i) = sum;
    }

    return sumHistogram;
}

Histogram PlotModel::discardInactiveDiscTypes(const Histogram& histogram) const
{
    const auto& categoryAxis = histogram.axis<0>();
    const auto& regularAxis = histogram.axis<1>();
    std::vector<std::string> activeDiscTypes;

    for (int i = 0; i < categoryAxis.size(); ++i)
    {
        const auto& discType = categoryAxis.value(i);
        if (activePlotDiscTypes_.at(discType))
            activeDiscTypes.push_back(discType);
    }

    Histogram filteredHistogram = makeHistogramWithCategories(histogram, activeDiscTypes);

    int category = 0;
    for (int i = 0; i < categoryAxis.size(); ++i)
    {
        const auto& discType = categoryAxis.value(i);
        if (!activePlotDiscTypes_.at(discType))
            continue;

        for (int j = 0; j < regularAxis.size(); ++j)
            filteredHistogram.at(category, j) = histogram.at(i, j);

        ++category;
    }

    return filteredHistogram;
}

Histogram PlotModel::makeHistogramWithCategories(const Histogram& source,
                                                 const std::vector<std::string>& categories) const
{
    const auto& regularAxis = source.axis<1>();

    return bh::make_histogram(bh::axis::category<std::string>(categories, "Disc types"),
                              bh::axis::regular<>(regularAxis.size(), regularAxis.value(0),
                                                  regularAxis.value(regularAxis.size()), regularAxis.metadata()));
}

DataPoint PlotModel::dataPointFromFrameDTO(const FrameDTO& frameDTO)
{
    const auto& discTypeRegistry = simulation_->getDiscTypeRegistry();
    DataPoint dataPoint(simulation_->getSimulationConfig());

    const auto& discs = frameDTO.discs_;

    for (const auto& [discType, collisionCount] : frameDTO.collisionCounts_)
        dataPoint.collisionCounts_[discTypeRegistry.getByID(discType).getName()] = static_cast<double>(collisionCount);

    dataPoint.elapsedTime_ = static_cast<double>(frameDTO.elapsedSimulationTimeUs) / 1'000'000.0;
    std::unordered_map<std::string, cell::Vector2d> momentumMap;

    for (const auto& disc : discs)
    {
        std::string discTypeName = discTypeRegistry.getByID(disc.getTypeID()).getName();
        ++dataPoint.discTypeCountMap_[discTypeName];
        dataPoint.totalKineticEnergyMap_[discTypeName] +=
            disc.getKineticEnergy(discTypeRegistry.getByID(disc.getTypeID()).getMass());
        momentumMap[discTypeName] += disc.getMomentum(discTypeRegistry.getByID(disc.getTypeID()).getMass());
        dataPoint.vxHistogram_(discTypeName, disc.getVelocity().x);
    }

    for (const auto& [discTypeName, momentum] : momentumMap)
        dataPoint.totalMomentumMap_[discTypeName] = cell::mathutils::abs(momentum);

    return dataPoint;
}

DataPoint& operator+=(DataPoint& lhs, const DataPoint& rhs)
{
    lhs.elapsedTime_ += rhs.elapsedTime_;

    utility::addMaps(lhs.collisionCounts_, rhs.collisionCounts_);
    utility::addMaps(lhs.discTypeCountMap_, rhs.discTypeCountMap_);
    utility::addMaps(lhs.totalKineticEnergyMap_, rhs.totalKineticEnergyMap_);
    utility::addMaps(lhs.totalMomentumMap_, rhs.totalMomentumMap_);
    lhs.vxHistogram_ += rhs.vxHistogram_;

    return lhs;
}