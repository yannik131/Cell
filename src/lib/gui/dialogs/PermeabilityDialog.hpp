#ifndef DBACDD6F_AD4A_44E4_993F_82747E3C5E8C_HPP
#define DBACDD6F_AD4A_44E4_993F_82747E3C5E8C_HPP

#include "core/Types.hpp"
#include "dialogs/TableViewDialog.hpp"
#include "models/PermeabilityTableModel.hpp"

class PermeabilityDialog : public TableViewDialog<PermeabilityTableModel::RowType>
{
    Q_OBJECT
    using Base = TableViewDialog<PermeabilityTableModel::RowType>;

public:
    PermeabilityDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);
    void setPermeabilityMap(const PermeabilityMap& permeabilityMap);
    PermeabilityMap getPermeabilityMap() const;
};

#endif /* DBACDD6F_AD4A_44E4_993F_82747E3C5E8C_HPP */
