#ifndef AB3F4088_AF2F_4472_8BA3_E8165A0B4B30_HPP
#define AB3F4088_AF2F_4472_8BA3_E8165A0B4B30_HPP

#include "cell/SimulationConfig.hpp"

#include <QAbstractTableModel>

class AbstractSimulationBuilder;

class DiscTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DiscTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void removeRow(int row);
    void addRow();
    void clearRows();
    const std::vector<cell::config::Disc>& getRows() const;
    void reload();

private:
    std::vector<cell::config::Disc> rows_;

    AbstractSimulationBuilder* abstractSimulationBuilder_;
};

#endif /* AB3F4088_AF2F_4472_8BA3_E8165A0B4B30_HPP */
