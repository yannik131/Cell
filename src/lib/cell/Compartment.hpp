#ifndef C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP
#define C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP

#include "Membrane.hpp"

#include <vector>

namespace cell
{

class Disc;

class Compartment
{
public:
    explicit Compartment(Membrane&& membrane);

    const Membrane& getMembrane() const;

private:
    Membrane membrane_;
    std::vector<Disc> discs_;
};

} // namespace cell

#endif /* C4819342_4F4C_446A_9CDF_CA4AA5E00883_HPP */
