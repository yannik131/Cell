#ifndef COMBOBOXDELEGATE_HPP
#define COMBOBOXDELEGATE_HPP

#include <QComboBox>
#include <QStyledItemDelegate>

class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

signals:
    void editorCreated(QComboBox* comboBox) const;
};

class DiscTypeComboBoxDelegate : public ComboBoxDelegate
{
    Q_OBJECT
public:
    DiscTypeComboBoxDelegate(QObject* parent = nullptr);
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override;
};

#endif /* COMBOBOXDELEGATE_HPP */
