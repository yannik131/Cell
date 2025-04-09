#ifndef BUTTONDELEGATE_HPP
#define BUTTONDELEGATE_HPP

#include <QStyledItemDelegate>

class ButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ButtonDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
                     const QModelIndex& index) override;

signals:
    void deleteRow(const QModelIndex& index) const;
};

#endif /* BUTTONDELEGATE_HPP */
