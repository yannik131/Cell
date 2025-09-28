#ifndef AE328161_6FB3_4EA0_8D38_CF7D51BEA90A_HPP
#define AE328161_6FB3_4EA0_8D38_CF7D51BEA90A_HPP

#include "Types.hpp"

namespace cell
{

class Compartment;
struct SimulationContext;

class Cell : public Compartment
{
public:
    Cell(Membrane&& membrane, std::vector<Membrane>&& membranes, SimulationContext simulationContext);

    /**
     * @returns The initial kinetic energy of all discs in this cell after `reinitialize()` was called
     */
    double getInitialKineticEnergy() const;

    /**
     * @brief The current kinetc energy of all discs in this cell
     */
    double getCurrentKineticEnergy() const;

private:
    // TODO remove, the simulation doesn't care about this
    double initialKineticEnergy_ = 0;
    double currentKineticEnergy_ = 0;
};

} // namespace cell

#endif /* AE328161_6FB3_4EA0_8D38_CF7D51BEA90A_HPP */
