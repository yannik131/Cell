#ifndef BC236487_5829_46C6_81D5_E7309D40F63E_HPP
#define BC236487_5829_46C6_81D5_E7309D40F63E_HPP

#include "models/PlotModel.hpp"

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
    ~PlotControlWidget();

    void setModel(PlotModel* plotModel);

signals:
    void selectDiscTypesClicked();

private:
    std::unique_ptr<Ui::PlotControlWidget> ui;
};

#endif /* BC236487_5829_46C6_81D5_E7309D40F63E_HPP */
