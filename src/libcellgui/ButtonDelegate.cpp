#include "ButtonDelegate.hpp"

#include <QApplication>
#include <QMouseEvent>
#include <QStyleOptionButton>

ButtonDelegate::ButtonDelegate(QObject* parent = nullptr)
    : QStyledItemDelegate(parent)
{
}

void ButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionButton buttonOption;
    buttonOption.rect = option.rect;
    buttonOption.text = "Delete";
    buttonOption.state = QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
}

bool ButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
                                 const QModelIndex& index)
{
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

    if (event->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::LeftButton &&
        option.rect.contains(mouseEvent->pos()))
        emit deleteRow(index);

    return true;
}