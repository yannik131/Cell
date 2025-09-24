#ifndef C5A7F999_3DE1_4DFF_8512_AD7C6C768848_HPP
#define C5A7F999_3DE1_4DFF_8512_AD7C6C768848_HPP

#include "cell/SimulationConfig.hpp"

#include <QAbstractTableModel>

class AbstractSimulationBuilder;

class DiscTypeDistributionTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DiscTypeDistributionTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder_);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void removeRow(int row);
    void addRow();
    void clearRows();
    const std::vector<std::pair<std::string, double>>& getRows() const;
    void reload();

private:
    std::vector<std::pair<std::string, double>> rows_;

    AbstractSimulationBuilder* abstractSimulationBuilder_;
};

#endif /* C5A7F999_3DE1_4DFF_8512_AD7C6C768848_HPP */
