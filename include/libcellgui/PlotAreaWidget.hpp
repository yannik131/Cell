#ifndef PLOTOVERVIEWWIDGET_HPP
#define PLOTOVERVIEWWIDGET_HPP

#include "AnalysisPlotWidget.hpp"
#include "PlotDataSelectionDialog.hpp"

#include <QComboBox>
#include <QPushButton>
#include <QWidget>

class PlotAreaWidget : public QWidget
{
    Q_OBJECT
public:
    PlotAreaWidget(QWidget* parent = nullptr);

private:
    void initializeWidgets();
};

#endif /* PLOTOVERVIEWWIDGET_HPP */
