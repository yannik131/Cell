#ifndef DISCDISTRIBUTIONPREVIEWTABLEMODEL_HPP
#define DISCDISTRIBUTIONPREVIEWTABLEMODEL_HPP

#include "DiscType.hpp"

#include <QAbstractTableModel>

class DiscDistributionPreviewTableModel : public QAbstractTableModel
{
public:
    DiscDistributionPreviewTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void loadSettings();

private:
    std::vector<std::pair<DiscType, int>> distribution_;
};

#endif /* DISCDISTRIBUTIONPREVIEWTABLEMODEL_HPP */
