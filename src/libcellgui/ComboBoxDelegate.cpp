#include "ComboBoxDelegate.hpp"
#include "GlobalSettingsFunctor.hpp"
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

DiscTypeComboBoxDelegate::ComboBoxDelegate(QObject* parent)
    : ComboBoxDelegate(parent)
{
}

QWidget* DiscTypeComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    auto* editor = new QComboBox(parent);
    editor->addItems(Utility::getDiscTypeNames());
    emit editorCreated(editor);

    connect(&GlobalSettingsFunctor::get(), GlobalSettingsFunctor::discTypeDistributionChanged,
            [=]()
            {
                editor->clear();
                editor->addItems(Utility::getDiscTypeNames());
            });

    return editor;
}
