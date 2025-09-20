#include "delegates/ButtonDelegate.hpp"

#include <QApplication>
#include <QMouseEvent>
#include <QStyleOptionButton>

ButtonDelegate::ButtonDelegate(QObject* parent, QString text)
    : QStyledItemDelegate(parent)
    , text_(std::move(text))
{
}

void ButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex&) const
{
    QStyleOptionButton buttonOption;
    buttonOption.rect = option.rect;
    buttonOption.text = text_;
    buttonOption.state = QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
}

bool ButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option,
                                 const QModelIndex& index)
{
    auto* mouseEvent = dynamic_cast<QMouseEvent*>(event);

    if (event->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::LeftButton &&
        option.rect.contains(mouseEvent->pos()))
        emit buttonClicked(index.row());

    return true;
}
