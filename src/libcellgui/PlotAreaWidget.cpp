#include "PlotAreaWidget.hpp"

#include <QTimer>

PlotAreaWidget::PlotAreaWidget(QWidget* parent)
    : QWidget(parent)
{
    QTimer::singleShot(0, this, &PlotAreaWidget::initializeWidgets);
}

void PlotAreaWidget::initializeWidgets()
{
    auto plotTypeComboBox = findChild<QComboBox*>("plotTypeComboBox");

    plotTypeComboBox->clear();
    plotTypeComboBox->addItems(SupportedPlotCategoryNames);
}
