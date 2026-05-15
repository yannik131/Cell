#include "SimulationRecordSerializer.hpp"
#include "DiscType.hpp"

#include <fstream>

namespace cell
{

void SimulationRecordSerializer::writeTypeCountsToCsv(const std::vector<DataPoint>& dataPoints,
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
        elapsedTime += dataPoint.elapsedTime_;
        file << elapsedTime.count();
        for (const auto& ID : discTypeIDs)
            file << "," << ((dataPoint.discTypeCounts_.contains(ID)) ? dataPoint.discTypeCounts_.at(ID) : 0);
        file << "\n";
    }
}

} // namespace cell