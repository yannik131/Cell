#ifndef C762E800_0127_40F6_B4E4_FFF3828C8895_HPP
#define C762E800_0127_40F6_B4E4_FFF3828C8895_HPP

#include "models/AbstractTableModel.hpp"

// For editing vectors in the config like membrane/disc types, reactions, membranes/discs
template <typename T> class AbstractSimulationConfigTableModel : public AbstractTableModel<T>
{
public:
    using AbstractTableModel<T>::AbstractTableModel<T>();

    virtual void loadFromConfig() = 0;
    virtual void saveToConfig() = 0;
};

#endif /* C762E800_0127_40F6_B4E4_FFF3828C8895_HPP */