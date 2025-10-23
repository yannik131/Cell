#ifndef C5574053_1640_43BC_AEE2_79C4C3B822E4_HPP
#define C5574053_1640_43BC_AEE2_79C4C3B822E4_HPP

#include "models/AbstractTableModel.hpp"

// For editing parts of a config, like permeabilities/distributions of membranes
template <typename T> class AbstractSimulationConfigPartTableModel : public AbstractTableModel<T>
{
public:
    using AbstractTableModel<T>::AbstractTableModel<T>();

    const std::vector<T>& getRows() const;
};

template <typename T> inline const std::vector<T>& AbstractSimulationConfigPartTableModel<T>::getRows() const
{
    return rows_;
}

#endif /* C5574053_1640_43BC_AEE2_79C4C3B822E4_HPP */
