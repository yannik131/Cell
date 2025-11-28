#include "delegates/ComboBoxDelegate.hpp"
#include "ComboBoxDelegate.hpp"
#include "core/SafeCast.hpp"
#include "core/Utility.hpp"

#include <QAbstractItemView>
#include <QPointer>

ComboBoxDelegate::ComboBoxDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

QWidget* ComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    auto* editor = new QComboBox(parent);
    emit editorCreated(editor);

    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    auto* comboBox = safeCast<QComboBox*>(editor);
    int i = comboBox->findText(value);
    if (i >= 0)
        comboBox->setCurrentIndex(i);
}

void ComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto* comboBox = safeCast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

DiscTypeComboBoxDelegate::DiscTypeComboBoxDelegate(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : ComboBoxDelegate(parent)
    , simulationConfigUpdater_(simulationConfigUpdater)
{
}

QWidget* DiscTypeComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    auto* editor = new QComboBox(parent);
    editor->addItems(getDiscTypeNames(simulationConfigUpdater_->getSimulationConfig().discTypes));
    emit editorCreated(editor);

    return editor;
}

QVector<QString> DiscTypeComboBoxDelegate::getDiscTypeNames(const std::vector<cell::config::DiscType>& discTypes) const
{
    QVector<QString> names;
    names.reserve(static_cast<qsizetype>(discTypes.size()));

    for (const auto& discType : discTypes)
        names.push_back(QString::fromStdString(discType.name));

    return names;
}

MembraneTypeComboBoxDelegate::MembraneTypeComboBoxDelegate(QObject* parent,
                                                           SimulationConfigUpdater* simulationConfigUpdater)
    : ComboBoxDelegate(parent)
    , simulationConfigUpdater_(simulationConfigUpdater)
{
}

QWidget* MembraneTypeComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&,
                                                    const QModelIndex&) const
{
    auto* editor = new QComboBox(parent);
    editor->addItems(getMembraneTypeNames(simulationConfigUpdater_->getSimulationConfig().membraneTypes));
    emit editorCreated(editor);

    return editor;
}

QVector<QString>
MembraneTypeComboBoxDelegate::getMembraneTypeNames(const std::vector<cell::config::MembraneType>& membraneTypes) const
{
    QVector<QString> names;
    names.reserve(static_cast<qsizetype>(membraneTypes.size()));

    for (const auto& membraneType : membraneTypes)
        names.push_back(QString::fromStdString(membraneType.name));

    return names;
}
