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
        int idx = comboBox->findText(value);
        if (idx >= 0)
            comboBox->setCurrentIndex(idx);
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
        if (event->type() == QEvent::MouseButtonRelease)
        {
            emit deleteRow(index);
        }
        return true;
    }
signals:
    void deleteRow(const QModelIndex& index) const;
};

struct RowData
{
    int spinValue;
    QString comboValue;
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
        return values.size();
    }
    int columnCount(const QModelIndex&) const override
    {
        return 4; // Updated to 4 columns
    }
    QVariant data(const QModelIndex& index, int role) const override
    {
        if (index.row() >= values.size())
            return {};
        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            const RowData& row = values.at(index.row());
            switch (index.column())
            {
            case 0: return row.spinValue;
            case 1: return row.spinValue; // Same as column 0
            case 2: return row.comboValue;
            case 3: return "Delete";
            }
        }
        return {};
    }
    bool setData(const QModelIndex& index, const QVariant& value, int role) override
    {
        if (index.row() >= values.size())
            return false;
        if (role == Qt::EditRole)
        {
            RowData& row = values[index.row()];
            switch (index.column())
            {
            case 0:
                row.spinValue = value.toInt();
                emit dataChanged(index, index);
                return true;
            case 2:
                row.comboValue = value.toString();
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
    void addRow(int value, const QString& combo)
    {
        beginInsertRows(QModelIndex(), values.size(), values.size());
        values.append({value, combo});
        endInsertRows();
    }
    void removeRow(int row)
    {
        if (row < 0 || row >= values.size())
            return;
        beginRemoveRows(QModelIndex(), row, row);
        values.removeAt(row);
        endRemoveRows();
    }

private:
    QList<RowData> values;
};

#include "Main.moc"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    CustomModel model;
    for (int i = 0; i < 100; ++i)
        model.addRow(i, QString(static_cast<char>((int)'A' + i % 3)));

    QTableView view;
    view.setModel(&model);

    SpinBoxDelegate spinDelegate;
    view.setItemDelegateForColumn(0, &spinDelegate);

    ComboBoxDelegate comboDelegate;
    view.setItemDelegateForColumn(2, &comboDelegate);

    ButtonDelegate buttonDelegate;
    view.setItemDelegateForColumn(3, &buttonDelegate);

    QObject::connect(&buttonDelegate, &ButtonDelegate::deleteRow,
                     [&](const QModelIndex& index) { model.removeRow(index.row()); });

    /*for (int row = 0; row < 100; ++row)
    {
        QModelIndex index = model.index(row, 0);
        view.openPersistentEditor(index);
    }*/

    view.show();
    return app.exec();
}