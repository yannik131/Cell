#ifndef ED290BD4_AB33_43F0_99DD_D27EBFF8712A_HPP
#define ED290BD4_AB33_43F0_99DD_D27EBFF8712A_HPP

#include "core/SimulationConfigUpdater.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QStyledItemDelegate>

/**
 * @brief Boilerplate combobox delegate
 */
class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ComboBoxDelegate(QObject* parent = nullptr);

    /**
     * @brief Creates a QComboBox widget on demand and emits an `editorCreated` signal
     */
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override;

    /**
     * @brief Sets the index of the QComboBox to the one of the selected text
     */
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    /**
     * @brief Calls `setModel` on the respective model with the current text
     */
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

signals:
    void editorCreated(QComboBox* comboBox) const;
};

/**
 * @brief Subclass of `ComboBoxDelegate` for displaying the disc type names from the disc type distribution in the
 * settings
 */
class DiscTypeComboBoxDelegate : public ComboBoxDelegate
{
    Q_OBJECT
public:
    DiscTypeComboBoxDelegate(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    /**
     * @brief Creates a `QComboBox` and fills it with the currently available disc type names
     */
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override;

private:
    QVector<QString> getDiscTypeNames(const std::vector<cell::config::DiscType>& discTypes) const;

private:
    SimulationConfigUpdater* simulationConfigUpdater_;
};

// TODO Fix DRY violation here once everything is running
class MembraneTypeComboBoxDelegate : public ComboBoxDelegate
{
    Q_OBJECT
public:
    MembraneTypeComboBoxDelegate(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override;

private:
    QVector<QString> getMembraneTypeNames(const std::vector<cell::config::MembraneType>& membraneTypes) const;

private:
    SimulationConfigUpdater* simulationConfigUpdater_;
};

template <std::integral... Columns>
void insertDiscTypeComboboxIntoView(QAbstractItemView* view, SimulationConfigUpdater* simulationConfigUpdater,
                                    Columns... columns)
{
    auto* discTypeComboBoxDelegate = new DiscTypeComboBoxDelegate(view, simulationConfigUpdater);
    (view->setItemDelegateForColumn(columns, discTypeComboBoxDelegate), ...);
    view->setEditTriggers(QAbstractItemView::EditTrigger::CurrentChanged |
                          QAbstractItemView::EditTrigger::SelectedClicked);
}

inline void insertMembraneTypeComboBoxIntoView(QAbstractItemView* view,
                                               SimulationConfigUpdater* simulationConfigUpdater, int column)
{
    auto* membraneTypeComboBoxDelegate = new MembraneTypeComboBoxDelegate(view, simulationConfigUpdater);
    view->setItemDelegateForColumn(column, membraneTypeComboBoxDelegate);
    view->setEditTriggers(QAbstractItemView::EditTrigger::CurrentChanged |
                          QAbstractItemView::EditTrigger::SelectedClicked);
}

#endif /* ED290BD4_AB33_43F0_99DD_D27EBFF8712A_HPP */
