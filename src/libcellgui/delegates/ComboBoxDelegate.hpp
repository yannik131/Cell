#ifndef COMBOBOXDELEGATE_HPP
#define COMBOBOXDELEGATE_HPP

#include <QComboBox>
#include <QStyledItemDelegate>

/**
 * @brief Boilerplate combobox delegate
 */
class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(QObject* parent = nullptr);

    /**
     * @brief Creates a QComboBox widget on demand and emits an `editorCreated` signal
     */
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override;

    /**
     * @brief Sets the index of the QComboBox to the one of the selected text
     */
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    /**
     * @brief Calls `setModel` on the respective model with the current text
     */
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

signals:
    void editorCreated(QComboBox* comboBox) const;
};

/**
 * @brief Subclass of `ComboBoxDelegate` for displaying the disc type names from the disc type distribution in the
 * settings
 */
class DiscTypeComboBoxDelegate : public ComboBoxDelegate
{
    Q_OBJECT
public:
    DiscTypeComboBoxDelegate(QObject* parent = nullptr);

    /**
     * @brief Creates a `QComboBox` and fills it with the currently available disc type names
     */
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override;
};

#endif /* COMBOBOXDELEGATE_HPP */
