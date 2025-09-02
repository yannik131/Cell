#include "widgets/MultiSelectListWidget.hpp"

#include <QMouseEvent>

MultiSelectListWidget::MultiSelectListWidget(QWidget*)
{
    setSelectionMode(QAbstractItemView::MultiSelection);
    setFocusPolicy(Qt::NoFocus);
    setStyleSheet("QListWidget::item:selected { background-color: blue; color: white; }");
}

void MultiSelectListWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    QListWidgetItem* item = itemAt(event->pos());
    if (item)
        item->setSelected(!item->isSelected());

    event->ignore();
}

void MultiSelectListWidget::mouseReleaseEvent(QMouseEvent* event)
{
    event->ignore();
}

void MultiSelectListWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    event->ignore();
}

void MultiSelectListWidget::mouseMoveEvent(QMouseEvent* event)
{
    event->ignore();
}

QStringList MultiSelectListWidget::getSelectedNames() const
{
    QStringList selectedNames;
    for (int i = 0; i < count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        if (item && item->isSelected())
            selectedNames.append(item->text());
    }
    return selectedNames;
}
