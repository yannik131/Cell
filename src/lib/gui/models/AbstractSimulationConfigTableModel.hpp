#ifndef B8F34530_1D0A_4E64_A6E6_CEEE9B21BBD5_HPP
#define B8F34530_1D0A_4E64_A6E6_CEEE9B21BBD5_HPP

#include "core/Types.hpp"

#include <QAbstractTableModel>

class AbstractConfigChanger
{
public:
    virtual void loadFromConfig() = 0;
    virtual void saveToConfig() = 0;
};

class SimulationConfigUpdater;

template <typename T> class AbstractSimulationConfigTableModel : public QAbstractTableModel
{
public:
    using RowType = T;

public:
    AbstractSimulationConfigTableModel(QObject* parent, const QStringList& headers,
                                       SimulationConfigUpdater* simulationConfigUpdater);

    // Necessary methods from QAbstractTableModel with default implementations
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // These require helpers to be implemented
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Necessary methods with default implementations
    virtual void removeRow(int row);
    virtual void clearRows();

    // Necessary methods without default implementations
    virtual void addRow() = 0;

    void setRows(std::vector<T> rows);
    const std::vector<T>& getRows() const;

protected:
    virtual QVariant getField(const T& row, int column) const = 0;        // for data(...)
    virtual bool setField(T& row, int column, const QVariant& value) = 0; // for setData(...)
    virtual bool isEditable(const QModelIndex& index) const = 0;          // for flags(...)

protected:
    std::vector<T> rows_;
    const QStringList headers_;
    SimulationConfigUpdater* simulationConfigUpdater_;
};

template <typename T>
inline AbstractSimulationConfigTableModel<T>::AbstractSimulationConfigTableModel(
    QObject* parent, const QStringList& headers, SimulationConfigUpdater* simulationConfigUpdater)
    : QAbstractTableModel(parent)
    , headers_(headers)
    , simulationConfigUpdater_(simulationConfigUpdater)
{
}

template <typename T> inline int AbstractSimulationConfigTableModel<T>::rowCount(const QModelIndex&) const
{
    return static_cast<int>(rows_.size());
}

template <typename T> inline int AbstractSimulationConfigTableModel<T>::columnCount(const QModelIndex&) const
{
    return static_cast<int>(headers_.size());
}

template <typename T>
inline QVariant AbstractSimulationConfigTableModel<T>::headerData(int section, Qt::Orientation orientation,
                                                                  int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    return headers_[section];
}

template <typename T>
inline QVariant AbstractSimulationConfigTableModel<T>::data(const QModelIndex& index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return {};
    if (!index.isValid() || index.row() >= static_cast<int>(rows_.size()))
        return {};

    return getField(rows_[index.row()], index.column());
}

template <typename T>
inline bool AbstractSimulationConfigTableModel<T>::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole || !index.isValid() || index.row() >= static_cast<int>(rows_.size()))
        return false;

    bool changed = setField(rows_[index.row()], index.column(), value);
    if (changed)
        emit dataChanged(index, index);

    return changed;
}

template <typename T> inline Qt::ItemFlags AbstractSimulationConfigTableModel<T>::flags(const QModelIndex& index) const
{
    auto flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (isEditable(index))
        flags |= Qt::ItemIsEditable;

    return flags;
}

template <typename T> inline void AbstractSimulationConfigTableModel<T>::removeRow(int row)
{
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    beginRemoveRows(QModelIndex(), row, row);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();
}

template <typename T> inline void AbstractSimulationConfigTableModel<T>::clearRows()
{
    beginResetModel();
    rows_.clear();
    endResetModel();
}

template <typename T> inline void AbstractSimulationConfigTableModel<T>::setRows(std::vector<T> rows)
{
    beginResetModel();
    rows_ = std::move(rows);
    endResetModel();
}

template <typename T> inline const std::vector<T>& AbstractSimulationConfigTableModel<T>::getRows() const
{
    return rows_;
}

#endif /* B8F34530_1D0A_4E64_A6E6_CEEE9B21BBD5_HPP */
