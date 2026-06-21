#include "SimulationRecordSerializer.hpp"
#include "DiscType.hpp"

#include <deque>
#include <fstream>

namespace cell
{

void SimulationRecordSerializer::writeTypeCountsToCsv(const std::deque<DataPoint>& dataPoints,
                                                      const DiscTypeRegistry& discTypeRegistry, const fs::path& outFile)
{
    std::ofstream file(outFile);
    if (!file)
        throw ExceptionWithLocation("Couldn't open file '" + outFile.string() + "' for writing");

    ch::duration<double> elapsedTime{};
    const auto& discTypes = discTypeRegistry.getValues();
    std::vector<DiscTypeID> discTypeIDs = discTypeRegistry.getIDs();

    file << "ElapsedTime[s]";
    for (const auto& discType : discTypes)
        file << "," << discType.getName();
    file << "\n";

    for (const auto& dataPoint : dataPoints)
    {
        elapsedTime += dataPoint.getData().elapsedTime;
        file << elapsedTime.count();
        for (const auto& ID : discTypeIDs)
            file << ","
                 << ((dataPoint.getData().discTypeCounts.contains(ID)) ? dataPoint.getData().discTypeCounts.at(ID) : 0);
        file << "\n";
    }
}

} // namespace cell