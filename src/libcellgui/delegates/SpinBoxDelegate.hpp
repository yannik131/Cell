#ifndef SPINBOXDELEGATE_HPP
#define SPINBOXDELEGATE_HPP

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QStyledItemDelegate>

#include <type_traits>

class SpinBoxDelegateBase : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

signals:
    void editorCreated(QWidget* spinBox) const;
};

template <typename SpinBoxType> class SpinBoxDelegate : public SpinBoxDelegateBase
{
public:
    static_assert(std::is_same_v<SpinBoxType, QSpinBox> || std::is_same_v<SpinBoxType, QDoubleSpinBox>,
                  "Template parameter must be QSpinBox or QDoubleSpinBox");

    using SpinBoxDelegateBase::SpinBoxDelegateBase;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        auto* editor = new SpinBoxType(parent);
        emit editorCreated(editor);
        return editor;
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        auto* spinBox = qobject_cast<SpinBoxType*>(editor);
        auto data = index.model()->data(index, Qt::EditRole);

        if constexpr (std::is_same_v<SpinBoxType, QSpinBox>)
            spinBox->setValue(data.toInt());
        else if constexpr (std::is_same_v<SpinBoxType, QDoubleSpinBox>)
            spinBox->setValue(data.toDouble());
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
    {
        auto* spinBox = qobject_cast<SpinBoxType*>(editor);
        model->setData(index, spinBox->value(), Qt::EditRole);
    }
};
#endif /* SPINBOXDELEGATE_HPP */
