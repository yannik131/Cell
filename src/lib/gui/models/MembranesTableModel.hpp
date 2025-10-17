#ifndef C1D94502_857B_4E2A_9378_0F448C9C4303_HPP
#define C1D94502_857B_4E2A_9378_0F448C9C4303_HPP

#include "cell/SimulationConfig.hpp"

#include <QAbstractTableModel>

// TODO Dry violation with DiscTableModel

class AbstractSimulationBuilder;

class MembranesTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MembranesTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void removeRow(int row);
    void addRow();
    void clearRows();
    const std::vector<cell::config::Membrane>& getRows() const;
    void reload();

private:
    std::vector<cell::config::Membrane> rows_;

    AbstractSimulationBuilder* abstractSimulationBuilder_;
};

#endif /* C1D94502_857B_4E2A_9378_0F448C9C4303_HPP */
