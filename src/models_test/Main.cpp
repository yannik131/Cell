#include <QAbstractTableModel>
#include <QApplication>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStyledItemDelegate>
#include <QTableView>

class SpinBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    SpinBoxDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        auto* editor = new QSpinBox(parent);
        return editor;
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        int value = index.model()->data(index, Qt::EditRole).toInt();
        auto* spinBox = qobject_cast<QSpinBox*>(editor);
        spinBox->setValue(value);
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
    {
        auto* spinBox = qobject_cast<QSpinBox*>(editor);
        model->setData(index, spinBox->value(), Qt::EditRole);
    }
};