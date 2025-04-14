#include "PlotControlWidget.hpp"
#include "GlobalGUISettings.hpp"
#include "PlotCategories.hpp"
#include "ui_PlotControlWidget.h"

PlotControlWidget::PlotControlWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PlotControlWidget)
{
    ui->plotTimeIntervalSpinBox->setRange(GUISettingsLimits::MinPlotTimeInterval,
                                          GUISettingsLimits::MaxPlotTimeInterval);
    ui->plotTimeIntervalSpinBox->setValue(GlobalGUISettings::getGUISettings().plotTimeInterval_.asMilliseconds());

    // Remove items set by the designer, might not be up to date (source code is the ssot)
    ui->plotTypeComboBox->clear();
    ui->plotTypeComboBox->addItems(SupportedPlotCategoryNames);
    comboBox->setCurrentIndex(options.indexOf(selectedOption));

    connect(ui->selectDiscTypesPushButton, &QPushButton::clicked, [this]() { emit selectDiscTypesClicked(); });
}