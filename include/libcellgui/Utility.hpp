#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QStringList>
#include <QTableView>

#include <type_traits>

namespace Utility
{

void setModelHeaderData(QStandardItemModel* model, const QStringList& headers);

template <typename SpinBoxType = QSpinBox>
SpinBoxType* addSpinBoxToLastRow(double value, double min, double max, QTableView* tableView, QStandardItemModel* model,
                                 int column, const QString& objectName = "")
{
    static_assert(std::is_base_of<QSpinBox, SpinBoxType>::value || std::is_base_of<QDoubleSpinBox, SpinBoxType>::value,
                  "T must be a subclass of QSpinBox or QDoubleSpinBox");

    SpinBoxType* spinBox = new SpinBoxType();
    if (!objectName.isEmpty())
        spinBox->setObjectName(objectName);

    if constexpr (std::is_base_of<QDoubleSpinBox, SpinBoxType>::value)
        spinBox->setDecimals(3);

    spinBox->setValue(value);
    spinBox->setRange(min, max);
    spinBox->setSingleStep(0.001);
    tableView->setIndexWidget(model->index(model->rowCount() - 1, column), spinBox);

    return spinBox;
}

/**
 * @brief Creates a QComboBox in the last row of the given tableView (assumes the row for the QComboBox was already
 * added)
 * @param options List of options for the QCombobox
 * @param selectedOptions Currently selected option
 * @param model The QStandardItemModel for the QTableView
 * @param tableView QTableView to create the widget in
 * @param column Column in which to put the QComboBox
 */
QComboBox* addComboBoxToLastRow(const QStringList& options, const QString& selectedOption, QStandardItemModel* model,
                                QTableView* tableView, int column);

}; // namespace Utility

#endif /* UTILITY_HPP */
