#ifndef DISCTYPEDISTRIBUTIONTABLEMODEL_HPP
#define DISCTYPEDISTRIBUTIONTABLEMODEL_HPP

#include "DiscType.hpp"

#include <QAbstractTableModel>

class DiscTypeDistributionTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DiscTypeDistributionTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    void addRowFromDiscType(const DiscType& discType);
    void removeRow(int row);

private:
    /**
     * @brief A row contains: Disc type name, radius, mass, color, frequency, delete button
     */
    std::vector<std::pair<DiscType, int>> rows_;
};

#endif /* DISCTYPEDISTRIBUTIONTABLEMODEL_HPP */
