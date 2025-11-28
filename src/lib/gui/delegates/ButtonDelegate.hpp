#ifndef AF2062F1_CA55_4ADB_BEC5_853F4C739FAF_HPP
#define AF2062F1_CA55_4ADB_BEC5_853F4C739FAF_HPP

#include "core/Types.hpp"

#include <QAbstractItemView>
#include <QDebug>
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

template <typename T> void insertDeleteButtonIntoView(T* model, QAbstractItemView* view, Column column)
{
    auto* deleteButtonDelegate = new ButtonDelegate(view, "Delete");
    QObject::connect(deleteButtonDelegate, &ButtonDelegate::buttonClicked, [model](int row) { model->removeRow(row); });
    view->setItemDelegateForColumn(column.value, deleteButtonDelegate);
}

#endif /* AF2062F1_CA55_4ADB_BEC5_853F4C739FAF_HPP */
