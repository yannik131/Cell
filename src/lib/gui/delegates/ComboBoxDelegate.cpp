#include "ComboBoxDelegate.hpp"
#include "SafeCast.hpp"
#include "Utility.hpp"

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

DiscTypeComboBoxDelegate::DiscTypeComboBoxDelegate(QObject* parent)
    : ComboBoxDelegate(parent)
{
}

QWidget* DiscTypeComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    auto* editor = new QComboBox(parent);
    editor->addItems(utility::getDiscTypeNames());
    emit editorCreated(editor);

    // TODO

    return editor;
}
