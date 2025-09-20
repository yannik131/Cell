#ifndef MULTISELECTLISTWIDGET_HPP
#define MULTISELECTLISTWIDGET_HPP

#include <QListWidget>

/**
 * @brief List widget that allows single-click multiple selections and does not automatically de-select everything else
 * when the user selects/deselects one item. Used for selecting data sets for plotting
 */
class MultiSelectListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit MultiSelectListWidget(QWidget* parent = nullptr);

    /**
     * @brief Returns a list of all currently selected entries
     */
    QStringList getSelectedNames() const;

protected:
    /**
     * @brief Toggles selection of a single element upon left mouse click
     */
    void mousePressEvent(QMouseEvent* event) override;

    // The other methods just ignore the event to disable the unwanted deselection behaviour

    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};

#endif /* MULTISELECTLISTWIDGET_HPP */
