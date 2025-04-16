#include "SpinBoxDelegate.hpp"

SpinBoxDelegate::SpinBoxDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

QWidget* SpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    auto* editor = new QSpinBox(parent);
    emit editorCreated(editor);

    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    auto* spinBox = qobject_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto* spinBox = qobject_cast<QSpinBox*>(editor);
    model->setData(index, spinBox->value(), Qt::EditRole);
}
