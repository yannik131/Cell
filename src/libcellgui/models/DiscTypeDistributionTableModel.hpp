#ifndef DISCTYPEDISTRIBUTIONTABLEMODEL_HPP
#define DISCTYPEDISTRIBUTIONTABLEMODEL_HPP

#include "DiscType.hpp"

#include <QAbstractTableModel>
#include <QVector>

class DiscTypeDistributionTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DiscTypeDistributionTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    void addRowFromDiscType(const DiscType& discType);
    void addEmptyRow();
    void removeRow(int row);
    void loadSettings();
    void saveSettings();
    void clearRows();

private:
    QVector<std::pair<DiscType, int>> rows_;
};

#endif /* DISCTYPEDISTRIBUTIONTABLEMODEL_HPP */
