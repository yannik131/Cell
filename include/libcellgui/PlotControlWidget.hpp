#ifndef PLOTCONTROLWIDGET_HPP
#define PLOTCONTROLWIDGET_HPP

#include <QWidget>

namespace Ui
{
class PlotControlWidget;
}

class PlotControlWidget : public QWidget
{
public:
    PlotControlWidget(QObject* parent = nullptr);

signals:
    void plotSettingsChanged();
    void selectDiscTypesClicked();

private:
    Ui::PlotControlWidget* ui;
};

#endif /* PLOTCONTROLWIDGET_HPP */
