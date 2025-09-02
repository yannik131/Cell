#ifndef DISCDISTRIBUTIONPREVIEWTABLEMODEL_HPP
#define DISCDISTRIBUTIONPREVIEWTABLEMODEL_HPP

#include <QAbstractTableModel>
#include <QVector>

/**
 * @brief Model for the little preview of currently available disc types in the main window
 */
class DiscDistributionPreviewTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /**
     * @brief Loads the settings and connects callbacks
     */
    explicit DiscDistributionPreviewTableModel(QObject* parent = nullptr);

    /**
     * @returns number of disc types in the last loaded disc type distribution
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @returns Number of columns of the preview table. Currently 3 (Type, color, percentage)
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @returns the header as a `QString` for the respective column
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /**
     * @returns The cell data for the respective disc type
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @brief The preview table should not be editable, so this function should make sure that that's the case
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;
};

#endif /* DISCDISTRIBUTIONPREVIEWTABLEMODEL_HPP */
