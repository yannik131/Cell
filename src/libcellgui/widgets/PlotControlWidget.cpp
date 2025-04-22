#include "PlotControlWidget.hpp"
#include "GlobalGUISettings.hpp"
#include "PlotCategories.hpp"
#include "ui_PlotControlWidget.h"

PlotControlWidget::PlotControlWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PlotControlWidget)
{
    ui->setupUi(this);

    ui->plotTimeIntervalSpinBox->setRange(GUISettingsLimits::MinPlotTimeInterval.asMilliseconds(),
                                          GUISettingsLimits::MaxPlotTimeInterval.asMilliseconds());
    ui->plotTimeIntervalSpinBox->setValue(GlobalGUISettings::getGUISettings().plotTimeInterval_.asMilliseconds());

    // Remove items set by the designer, might not be up to date (source code is the ssot)
    ui->plotTypeComboBox->clear();
    ui->plotTypeComboBox->addItems(SupportedPlotCategoryNames);
    ui->plotTypeComboBox->setCurrentIndex(
        SupportedPlotCategories.indexOf(GlobalGUISettings::getGUISettings().currentPlotCategory_));

    connect(ui->selectDiscTypesPushButton, &QPushButton::clicked, [this]() { emit selectDiscTypesClicked(); });
    connect(ui->plotTimeIntervalSpinBox, &QSpinBox::valueChanged,
            [](int value) { GlobalGUISettings::get().setPlotTimeInterval(sf::milliseconds(value)); });
    connect(ui->plotTypeComboBox, &QComboBox::currentIndexChanged,
            [](int index) { GlobalGUISettings::get().setCurrentPlotCategory(SupportedPlotCategories[index]); });
}
