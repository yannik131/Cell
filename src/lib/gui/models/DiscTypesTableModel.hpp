#ifndef B820F75C_97AE_4E27_9359_BBAADC72724F_HPP
#define B820F75C_97AE_4E27_9359_BBAADC72724F_HPP

#include "cell/SimulationConfig.hpp"
#include "models/AbstractSimulationConfigTableModel.hpp"

#include <SFML/Graphics/Color.hpp>

#include <unordered_set>

class DiscTypesTableModel : public AbstractSimulationConfigTableModel<cell::config::DiscType>,
                            public AbstractConfigChanger
{
    Q_OBJECT
    using Base = AbstractSimulationConfigTableModel<cell::config::DiscType>;

public:
    DiscTypesTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void removeRow(int row) override;
    void clearRows() override;
    void addRow() override;
    void loadFromConfig() override;
    void saveToConfig() override;

private:
    QVariant getField(const cell::config::DiscType& row, const QModelIndex& index) const override;
    bool setField(cell::config::DiscType& row, const QModelIndex& index, const QVariant& value) override;
    bool isEditable(const QModelIndex& index) const override;

private:
    std::vector<sf::Color> discColors_;
    std::unordered_set<std::string> removedDiscTypes_;
};

#endif /* B820F75C_97AE_4E27_9359_BBAADC72724F_HPP */
