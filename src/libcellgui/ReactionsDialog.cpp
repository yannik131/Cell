#include "ReactionsDialog.hpp"
#include "Utility.hpp"

ReactionsDialog::ReactionsDialog(QWidget* parent)
{
    Utility::setModelHeaderData(reactionsModel_, {"A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"});
    ui->reactionsTableView->setModel(reactionsModel_);
}