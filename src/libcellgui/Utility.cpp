#include "Utility.hpp"

#include <QComboBox>
#include <QTableView>

namespace Utility
{

void setModelHeaderData(QStandardItemModel* model, const QStringList& headers)
{
    model->setColumnCount(headers.size());

    for (int i = 0; i < headers.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
}

void addComboBoxToLastRow(const QStringList& options, const QString& selectedOption, QStandardItemModel* model,
                          QTableView* tableView, int column)
{
    QComboBox* comboBox = new QComboBox();
    comboBox->addItems(options);
    comboBox->setCurrentIndex(options.indexOf(selectedOption));
    tableView->setIndexWidget(model->index(model->rowCount() - 1, column), comboBox);
}

} // namespace Utility
