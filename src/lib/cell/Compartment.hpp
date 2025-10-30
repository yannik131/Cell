#ifndef C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP
#define C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP

#include "Membrane.hpp"
#include "SimulationContext.hpp"

#include <list>
#include <vector>

namespace cell
{

class Disc;

class Compartment
{
public:
    Compartment(Compartment* parent, Membrane membrane, SimulationContext simulationContext);
    ~Compartment();

    const Membrane& getMembrane() const;
    void setDiscs(std::vector<Disc>&& discs);
    void addDisc(Disc disc);
    const std::vector<Disc>& getDiscs() const;
    void addIntrudingDisc(Disc& disc);
    std::vector<std::unique_ptr<Compartment>>& getCompartments();
    const std::vector<std::unique_ptr<Compartment>>& getCompartments() const;
    const Compartment* getParent() const;
    void update(double dt);
    Compartment* createSubCompartment(Membrane membrane);

private:
    void moveDiscsAndApplyUnimolecularReactions(double dt);
    auto detectCollisions();
    void moveDiscsIntoChildCompartments(auto& discMembraneCollisions);
    void moveDiscsIntoParentCompartment();
    void updateChildCompartments(double dt);

    /**
     * @brief Removed all discs that were marked as destroyed (i. e. after decomposition or combination reactions) and
     * calculates the current kinetic energy based on the discs that are still in the cell
     */
    void removeDestroyedDiscs();

private:
    Compartment* parent_;
    Membrane membrane_;
    std::vector<Disc> discs_;
    std::vector<Disc*> intrudingDiscs_;
    std::vector<std::unique_ptr<Compartment>> compartments_; // There are references to these elements (parent)
    std::vector<Membrane> membranes_;
    SimulationContext simulationContext_;
};

} // namespace cell

#endif /* C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP */
