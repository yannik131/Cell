#ifndef D7A2ACDF_B39A_4AA7_9906_03973244209E_HPP
#define D7A2ACDF_B39A_4AA7_9906_03973244209E_HPP

#include "cell/SimulationConfig.hpp"

#include <QAbstractTableModel>

class AbstractSimulationBuilder;

class PermeabilityTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PermeabilityTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    void setPermeabilityMap(std::unordered_map<std::string, cell::MembraneType::Permeability>& permeabilityMap);
    void commitChanges();

private:
    void convertMapToRows(const std::unordered_map<std::string, cell::MembraneType::Permeability>& permeabilityMap);

private:
    std::vector<cell::MembraneType::Permeability> rows_;
    std::unordered_map<std::string, cell::MembraneType::Permeability>* permeabilityMap_ = nullptr;

    AbstractSimulationBuilder* abstractSimulationBuilder_;
};

#endif /* D7A2ACDF_B39A_4AA7_9906_03973244209E_HPP */
