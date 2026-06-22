#ifndef DAACCE91_83E5_4E02_8F73_BE09DE3EAC89_HPP
#define DAACCE91_83E5_4E02_8F73_BE09DE3EAC89_HPP

#include "DataPoint.hpp"
#include "Types.hpp"

#include <deque>

namespace cell
{

class SimulationRecordSerializer
{
public:
    void writeTypeCountsToCsv(const std::deque<DataPoint>& dataPoints, const DiscTypeRegistry& discTypeRegistry,
                              const fs::path& outFile);
};

} // namespace cell

#endif /* DAACCE91_83E5_4E02_8F73_BE09DE3EAC89_HPP */
