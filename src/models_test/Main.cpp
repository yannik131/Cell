#include <QAbstractTableModel>
#include <QApplication>
#include <QComboBox>
#include <QMouseEvent>
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

class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        auto* editor = new QComboBox(parent);
        editor->addItems({"A", "B", "C"});
        return editor;
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        auto* comboBox = qobject_cast<QComboBox*>(editor);
        int i = comboBox->findText(value);
        if (i >= 0)
            comboBox->setCurrentIndex(i);
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
    {
        auto* comboBox = qobject_cast<QComboBox*>(editor);
        model->setData(index, comboBox->currentText(), Qt::EditRole);
    }
};

class ButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ButtonDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyleOptionButton buttonOption;
        buttonOption.rect = option.rect;
        buttonOption.text = "Delete";
        buttonOption.state = QStyle::State_Enabled;

        QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
    }

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
                     const QModelIndex& index) override
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if (event->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::LeftButton &&
            option.rect.contains(mouseEvent->pos()))
            emit deleteRow(index);

        return true;
    }
signals:
    void deleteRow(const QModelIndex& index) const;
};

struct RowData
{
    int spinBoxValue;
    QString comboBoxValue;
};

class CustomModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    CustomModel(QObject* parent = nullptr)
        : QAbstractTableModel(parent)
    {
    }

    int rowCount(const QModelIndex&) const override
    {
        return rows_.size();
    }

    int columnCount(const QModelIndex&) const override
    {
        return 4;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (index.row() >= rows_.size())
            return {};

        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            const RowData& row = rows_.at(index.row());
            switch (index.column())
            {
            case 0: return row.spinBoxValue;
            case 1: return row.spinBoxValue;
            case 2: return row.comboBoxValue;
            case 3: return "Delete";
            }
        }

        return {};
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role) override
    {
        if (index.row() >= rows_.size())
            return false;

        if (role == Qt::EditRole)
        {
            RowData& row = rows_[index.row()];
            switch (index.column())
            {
            case 0:
                row.spinBoxValue = value.toInt();
                emit dataChanged(index, index);
                return true;
            case 2:
                row.comboBoxValue = value.toString();
                emit dataChanged(index, index);
                return true;
            }
        }

        return false;
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        if (index.column() == 0 || index.column() == 2)
            return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        else
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    void addRow(int spinBoxValue, const QString& comboBoxValue)
    {
        beginInsertRows(QModelIndex(), rows_.size(), rows_.size());
        rows_.append({spinBoxValue, comboBoxValue});
        endInsertRows();
    }

    void removeRow(int row)
    {
        if (row < 0 || row >= rows_.size())
            return;

        beginRemoveRows(QModelIndex(), row, row);
        rows_.removeAt(row);
        endRemoveRows();
    }

private:
    QVector<RowData> rows_;
};

#include "Main.moc"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    CustomModel model;
    for (int i = 0; i < 100; ++i)
        model.addRow(i, QString(static_cast<char>(static_cast<int>('A') + i % 3)));

    QTableView view;

    // Hover effect for ButtonDelegate
    view.viewport()->setAttribute(Qt::WA_Hover, true);
    // Editable after single click
    view.setEditTriggers(QAbstractItemView::EditTrigger::CurrentChanged);

    view.setModel(&model);

    SpinBoxDelegate spinBoxDelegate(&view);
    view.setItemDelegateForColumn(0, &spinBoxDelegate);

    ComboBoxDelegate comboBoxDelegate(&view);
    view.setItemDelegateForColumn(2, &comboBoxDelegate);

    ButtonDelegate buttonDelegate(&view);
    view.setItemDelegateForColumn(3, &buttonDelegate);

    QObject::connect(&buttonDelegate, &ButtonDelegate::deleteRow,
                     [&](const QModelIndex& index) { model.removeRow(index.row()); });

    view.show();
    return app.exec();
}