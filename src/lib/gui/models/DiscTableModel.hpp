#ifndef AB3F4088_AF2F_4472_8BA3_E8165A0B4B30_HPP
#define AB3F4088_AF2F_4472_8BA3_E8165A0B4B30_HPP

#include "cell/SimulationConfig.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "models/AbstractSimulationConfigTableModel.hpp"

class DiscTableModel : public AbstractSimulationConfigTableModel<cell::config::Disc>
{
    Q_OBJECT
public:
    DiscTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void addRow() override;
    void reload() override;

private:
    QVariant getField(const cell::config::Disc& row, int column) const override;
    bool setField(cell::config::Disc& row, int column, const QVariant& value) override;
    bool isEditable(const QModelIndex& index) const override;

    const std::vector<cell::config::Disc>& getRows() const;
};

#endif /* AB3F4088_AF2F_4472_8BA3_E8165A0B4B30_HPP */
