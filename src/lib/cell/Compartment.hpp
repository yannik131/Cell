#ifndef C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP
#define C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP

#include "CollisionDetector.hpp"
#include "Membrane.hpp"
#include "SimulationContext.hpp"

#include <deque>
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

    Compartment& operator=(const Compartment&) = delete;
    Compartment(const Compartment&) = delete;
    Compartment& operator=(Compartment&&) = delete;
    Compartment(Compartment&&) = delete;

    const Membrane& getMembrane() const;
    void setDiscs(std::deque<Disc>&& discs);
    void addDisc(Disc disc);
    const std::deque<Disc>& getDiscs() const;
    void addIntrudingDisc(Disc& disc, SearchChildren searchChildren);
    std::vector<std::unique_ptr<Compartment>>& getCompartments();
    const std::vector<std::unique_ptr<Compartment>>& getCompartments() const;
    const Compartment* getParent() const;
    void update(double dt);
    Compartment* createSubCompartment(Membrane membrane);

private:
    auto detectDiscMembraneCollisions();
    auto detectDiscDiscCollisions();
    void moveDiscsBetweenCompartments(const std::vector<CollisionDetector::Collision>& discMembraneCollisions);
    void updateChildCompartments(double dt);
    void moveDiscsAndCleanUp(double dt);
    void bimolecularUpdate();
    void unimolecularUpdate(double dt);

private:
    Compartment* parent_;
    Membrane membrane_;
    std::deque<Disc> discs_;
    std::vector<Disc*> intrudingDiscs_;
    std::vector<std::unique_ptr<Compartment>> compartments_; // There are references to these elements (parent)
    std::vector<Membrane> membranes_;
    SimulationContext simulationContext_;
    CollisionDetector collisionDetector_;
};

} // namespace cell

#endif /* C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP */
