#ifndef B8F34530_1D0A_4E64_A6E6_CEEE9B21BBD5_HPP
#define B8F34530_1D0A_4E64_A6E6_CEEE9B21BBD5_HPP

#include "core/Types.hpp"

#include <QAbstractTableModel>

class SimulationConfigUpdater;

template <typename T> class AbstractTableModel : public QAbstractTableModel
{
public:
    AbstractTableModel(QObject* parent, const QStringList& headers, SimulationConfigUpdater* simulationConfigUpdater);

    // Necessary methods from QAbstractTableModel with default implementations
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // These require helpers to be implemented
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Necessary methods with default implementations
    void removeRow(int row);
    void clearRows();

    // Necessary methods without default implementations
    virtual void addRow() = 0;

    void setRows(std::vector<T> rows);

protected:
    virtual QVariant getField(const T& row, int column) const = 0;        // Helper for data(...)
    virtual bool setField(T& row, int column, const QVariant& value) = 0; // Helper for setData(...)
    virtual bool isEditable(const QModelIndex& index) const = 0;          // Helper for flags(...)

protected:
    std::vector<T> rows_;
    const QStringList headers_;
    SimulationConfigUpdater* simulationConfigUpdater_;
};

template <typename T>
inline AbstractTableModel<T>::AbstractTableModel(QObject* parent, const QStringList& headers,
                                                 SimulationConfigUpdater* simulationConfigUpdater)
    : QAbstractTableModel(parent)
    , headers_(headers)
    , simulationConfigUpdater_(simulationConfigUpdater)
{
}

template <typename T> inline int AbstractTableModel<T>::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(rows_.size());
}

template <typename T> inline int AbstractTableModel<T>::columnCount(const QModelIndex& parent) const
{
    return static_cast<int>(headers_.size());
}

template <typename T>
inline QVariant AbstractTableModel<T>::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    return headers_[section];
}

template <typename T> inline QVariant AbstractTableModel<T>::data(const QModelIndex& index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return {};
    if (!index.isValid() || index.row() >= static_cast<int>(rows_.size()))
        return {};

    return getField(rows_[index.row()], index.column());
}

template <typename T>
inline bool AbstractTableModel<T>::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole || !index.isValid() || index.row() >= static_cast<int>(rows_.size()))
        return false;

    bool changed = setField(rows_[index.row()], index.column(), value);
    if (changed)
        emit dataChanged(index, index);

    return changed;
}

template <typename T> inline Qt::ItemFlags AbstractTableModel<T>::flags(const QModelIndex& index) const
{
    auto flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (isEditable(index))
        flags |= Qt::ItemIsEditable;

    return flags;
}

template <typename T> inline void AbstractTableModel<T>::removeRow(int row)
{
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    beginRemoveRows(QModelIndex(), row, row);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();
}

template <typename T> inline void AbstractTableModel<T>::clearRows()
{
    beginResetModel();
    rows_.clear();
    endResetModel();
}

template <typename T> inline void AbstractTableModel<T>::setRows(std::vector<T> rows)
{
    beginResetModel();
    rows_ = std::move(rows);
    endResetModel();
}

#endif /* B8F34530_1D0A_4E64_A6E6_CEEE9B21BBD5_HPP */
