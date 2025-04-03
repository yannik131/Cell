#ifndef PLOTOVERVIEWWIDGET_HPP
#define PLOTOVERVIEWWIDGET_HPP

#include "AnalysisPlotWidget.hpp"
#include "PlotDataSelectionDialog.hpp"

#include <QComboBox>
#include <QPushButton>
#include <QWidget>

class PlotOverviewWidget : public QWidget
{
    Q_OBJECT
public:
    PlotOverviewWidget(QWidget* parent = nullptr);

private slots:
    void onPlotTypeComboBoxCurrentIndexChanged(int index);

private:
    void init();
    void initializeWidgets();
    void setCallbacks();

private:
    QComboBox* plotTypeComboBox_;
    QPushButton* selectDiscTypesPushButton_;
    AnalysisPlotWidget* analysisPlotWidget_;
    PlotDataSelectionDialog* plotDataSelectionDialog_;
};

#endif /* PLOTOVERVIEWWIDGET_HPP */
