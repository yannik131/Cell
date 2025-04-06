#ifndef DISCTYPEDISTRIBUTIONTABLEMODEL_HPP
#define DISCTYPEDISTRIBUTIONTABLEMODEL_HPP

#include <QAbstractTableModel>

class DiscTypeDistributionTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DiscTypeDistributionTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
};

#endif /* DISCTYPEDISTRIBUTIONTABLEMODEL_HPP */
