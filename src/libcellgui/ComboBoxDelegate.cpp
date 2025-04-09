#include "ComboBoxDelegate.hpp"

#include <QComboBox>

ComboBoxDelegate::ComboBoxDelegate(QObject* parent = nullptr)
    : QStyledItemDelegate(parent)
{
}

QWidget* ComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    auto* editor = new QComboBox(parent);
    editor->addItems(availableItems_);
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    auto* comboBox = qobject_cast<QComboBox*>(editor);
    int i = comboBox->findText(value);
    if (i >= 0)
        comboBox->setCurrentIndex(i);
}

void ComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto* comboBox = qobject_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

void ComboBoxDelegate::setAvailableItems(const QStringList& availableItems)
{
    availableItems_ = availableItems;
}
