#include "SpinBoxDelegate.hpp"

#include <QAbstractItemView>

void insertProbabilitySpinBoxIntoView(QAbstractItemView* view, Column column)
{
    insertDoubleSpinBoxIntoView(
        view, DoubleSpinBoxParams{.column = column.value, .min = 0, .max = 1, .step = 1e-6, .decimals = 6});
}

void insertDoubleSpinBoxIntoView(QAbstractItemView* view, const DoubleSpinBoxParams& params)
{
    using SpinBoxDelegate = SpinBoxDelegate<QDoubleSpinBox>;
    auto* spinBox = new SpinBoxDelegate(view);

    QObject::connect(spinBox, &SpinBoxDelegate::editorCreated,
                     [params](QWidget* spinBox)
                     {
                         const double min = params.min ? *params.min : -1e99;
                         const double max = params.max ? *params.max : 1e99;

                         safeCast<QDoubleSpinBox*>(spinBox)->setRange(min, max);
                         safeCast<QDoubleSpinBox*>(spinBox)->setSingleStep(params.step);
                         safeCast<QDoubleSpinBox*>(spinBox)->setDecimals(params.decimals);
                     });

    view->setItemDelegateForColumn(params.column, spinBox);
}

void insertIntegerSpinBoxIntoView(QAbstractItemView* view, const IntegerSpinBoxParams& params)
{
    using SpinBoxDelegate = SpinBoxDelegate<QSpinBox>;
    auto* spinBox = new SpinBoxDelegate(view);

    QObject::connect(spinBox, &SpinBoxDelegate::editorCreated,
                     [params](QWidget* spinBox)
                     {
                         safeCast<QSpinBox*>(spinBox)->setRange(params.min, params.max);
                         safeCast<QSpinBox*>(spinBox)->setSingleStep(params.step);
                     });

    view->setItemDelegateForColumn(params.column, spinBox);
}
