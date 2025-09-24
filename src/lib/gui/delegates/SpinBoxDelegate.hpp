#ifndef B08BC3D3_C04D_4F93_BB91_8575673B2CBF_HPP
#define B08BC3D3_C04D_4F93_BB91_8575673B2CBF_HPP

#include "core/SafeCast.hpp"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QStyledItemDelegate>

#include <type_traits>

/**
 * @brief Classes with Q_OBJECT can't be templates. However, classes deriving from a class with Q_OBJECT can, so this is
 * an empty base class defining the signals the actual class will use
 */
class SpinBoxDelegateBase : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

signals:
    void editorCreated(QWidget* spinBox) const;
};

/**
 * @brief Template class for QSpinBox and QDoubleSpinBox delegates
 */
template <typename SpinBoxType> class SpinBoxDelegate : public SpinBoxDelegateBase
{
public:
    static_assert(std::is_same_v<SpinBoxType, QSpinBox> || std::is_same_v<SpinBoxType, QDoubleSpinBox>,
                  "Template parameter must be QSpinBox or QDoubleSpinBox");

    using SpinBoxDelegateBase::SpinBoxDelegateBase;

    /**
     * @brief Creates the spin box and emits the `editorCreated` signal
     */
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        auto* editor = new SpinBoxType(parent);
        emit editorCreated(editor);
        return editor;
    }

    /**
     * @brief Sets the current value of the spin box to the one given by the model, using the appropriate type for the
     * specified spin box type
     */
    void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        auto* spinBox = safeCast<SpinBoxType*>(editor);

        auto data = index.model()->data(index, Qt::EditRole);

        if constexpr (std::is_same_v<SpinBoxType, QSpinBox>)
            spinBox->setValue(data.toInt());
        else if constexpr (std::is_same_v<SpinBoxType, QDoubleSpinBox>)
            spinBox->setValue(data.toDouble());
    }

    /**
     * @brief Calls `setData` on the respective model with the current value
     */
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
    {
        auto* spinBox = safeCast<SpinBoxType*>(editor);

        model->setData(index, spinBox->value(), Qt::EditRole);
    }
};

void insertProbabilitySpinBoxIntoView(QAbstractItemView* view, int column);

#endif /* B08BC3D3_C04D_4F93_BB91_8575673B2CBF_HPP */
