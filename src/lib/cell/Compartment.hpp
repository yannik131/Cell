#ifndef C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP
#define C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP

#include "CollisionDetector.hpp"
#include "Membrane.hpp"
#include "SimulationContext.hpp"

#include <list>
#include <vector>

namespace cell
{

class Disc;

class Compartment
{
private:
    struct CompartmentEntry
    {
        double rightX;
        const MembraneType* membraneType;
        const Membrane* membrane;
        Compartment* compartment;
    };

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
    void updateChildCompartments(double dt);

private:
    Compartment* parent_;
    Membrane membrane_;
    std::vector<Disc> discs_;
    std::vector<Disc*> intrudingDiscs_;
    std::vector<std::unique_ptr<Compartment>> compartments_; // There are references to these elements (parent)
    std::vector<CompartmentEntry> compartmentEntries_;
    std::vector<Membrane> membranes_;
    SimulationContext simulationContext_;
};

} // namespace cell

#endif /* C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP */
