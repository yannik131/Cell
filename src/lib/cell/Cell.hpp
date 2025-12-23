#ifndef AE328161_6FB3_4EA0_8D38_CF7D51BEA90A_HPP
#define AE328161_6FB3_4EA0_8D38_CF7D51BEA90A_HPP

#include "Compartment.hpp"

namespace cell
{

struct SimulationContext;

class Cell : public Compartment
{
public:
    Cell(Membrane membrane, SimulationContext simulationContext);
};

} // namespace cell

#endif /* AE328161_6FB3_4EA0_8D38_CF7D51BEA90A_HPP */
