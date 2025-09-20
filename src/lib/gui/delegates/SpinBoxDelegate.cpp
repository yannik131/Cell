#include "SpinBoxDelegate.hpp"

#include <QAbstractItemView>

void insertProbabilitySpinBoxIntoView(QAbstractItemView* view, int column)
{
    using SpinBoxDelegate = SpinBoxDelegate<QDoubleSpinBox>;
    auto* probabilitySpinBoxDelegate = new SpinBoxDelegate(view);

    QObject::connect(probabilitySpinBoxDelegate, &SpinBoxDelegate::editorCreated,
                     [](QWidget* spinBox)
                     {
                         safeCast<QDoubleSpinBox*>(spinBox)->setRange(0.0, 1.0);
                         safeCast<QDoubleSpinBox*>(spinBox)->setSingleStep(0.001);
                         safeCast<QDoubleSpinBox*>(spinBox)->setDecimals(3);
                     });

    view->setItemDelegateForColumn(column, probabilitySpinBoxDelegate);
}