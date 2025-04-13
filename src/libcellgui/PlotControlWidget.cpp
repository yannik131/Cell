#include "PlotControlWidget.hpp"
#include "GlobalGUISettings.hpp"
#include "PlotCategories.hpp"
#include "ui_PlotControlWidget.h"

PlotControlWidget::PlotControlWidget(QObject* parent)
    : QWidget(parent)
    , ui(new Ui::PlotControlWidget(this))
{
    ui->plotTimeIntervalSpinBox->setRange(GUISettingsLimits::MinPlotTimeInterval,
                                          GUISettingsLimits::MaxPlotTimeInterval);
    ui->plotTimeIntervalSpinBox->setValue(GlobalGUISettings::getGUISettings().plotTimeInterval_.asMilliseconds());

    // Remove items set by the designer, might not be up to date (source code is the ssot)
    ui->plotTypeComboBox->clear();
    ui->plotTypeComboBox->addItems(SupportedPlotCategoryNames);
    comboBox->setCurrentIndex(options.indexOf(selectedOption));
}