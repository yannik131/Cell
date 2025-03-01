#include "Utility.hpp"

namespace Utility
{

void setModelHeaderData(QStandardItemModel* model, const QStringList& headers)
{
    model->setColumnCount(headers.size());

    for (int i = 0; i < headers.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
}

} // namespace Utility
