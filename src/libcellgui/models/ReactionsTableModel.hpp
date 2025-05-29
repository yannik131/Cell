#ifndef REACTIONSTABLEMODEL_HPP
#define REACTIONSTABLEMODEL_HPP

#include "Reaction.hpp"

#include <QAbstractTableModel>

class ReactionsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ReactionsTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    void addRowFromReaction(const Reaction& reaction);
    void addEmptyRow(const Reaction::Type& type);
    void removeRow(int row);
    void loadSettings();
    void saveSettings();
    void clearRows();

signals:
    void reactionsChanged();

private:
    std::vector<Reaction> rows_;
};

#endif /* REACTIONSTABLEMODEL_HPP */
