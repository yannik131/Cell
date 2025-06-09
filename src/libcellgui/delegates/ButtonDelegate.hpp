#ifndef BUTTONDELEGATE_HPP
#define BUTTONDELEGATE_HPP

#include <QStyledItemDelegate>

/**
 * @brief Boilerplate button delegate to display buttons in table views
 */
class ButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    /**
     * @brief Creates a button delegate of a push button that will display the given text
     */
    ButtonDelegate(QObject* parent, QString text);

    /**
     * @brief Paints the button with the specified text
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * @brief Reacts to a left mouse click release and emits the buttonClicked signal with the index of the row the
     * button is inside of as an argument
     */
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
                     const QModelIndex& index) override;

signals:
    void buttonClicked(int row) const;

private:
    QString text_;
};

#endif /* BUTTONDELEGATE_HPP */
