#ifndef PLOTCONTROLWIDGET_HPP
#define PLOTCONTROLWIDGET_HPP

#include "PlotModel.hpp"

#include <QWidget>

namespace Ui
{
class PlotControlWidget;
}

/**
 * @brief Widget containing widgets to modify the plot (time interval etc.)
 */
class PlotControlWidget : public QWidget
{
    Q_OBJECT
public:
    PlotControlWidget(QWidget* parent = nullptr);

signals:
    void plotSettingsChanged();
    void selectDiscTypesClicked();

private:
    Ui::PlotControlWidget* ui;
};

#endif /* PLOTCONTROLWIDGET_HPP */
