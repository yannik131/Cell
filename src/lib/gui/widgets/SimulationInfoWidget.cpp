#include "widgets/SimulationInfoWidget.hpp"
#include "SimulationInfoWidget.hpp"
#include "cell/StringUtils.hpp"
#include "ui_SimulationInfoWidget.h"

SimulationInfoWidget::SimulationInfoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SimulationInfoWidget)
{
    ui->setupUi(this);
}

SimulationInfoWidget::~SimulationInfoWidget() = default;

void SimulationInfoWidget::setRenderData(int targetFPS, int actualFPS, std::chrono::nanoseconds renderTime)
{
    ui->targetFPSLabel->setText(QString("Target FPS: %1").arg(targetFPS));
    ui->actualFPSLabel->setText(QString("Actual FPS: %1").arg(actualFPS));

    const std::string renderTimeString = cell::stringutils::timeString(renderTime.count(), 3);
    ui->renderTimeLabel->setText(QString("Render time: %1").arg(QString::fromStdString(renderTimeString)));
}

void SimulationInfoWidget::setSimulationData(double targetScale, double actualScale,
                                             std::chrono::nanoseconds updateTime,
                                             std::chrono::nanoseconds simulationUpdateTime)
{
    ui->targetScaleLabel->setText(QString("Target scale: %1").arg(targetScale, 0, 'f', 3));
    ui->actualScaleLabel->setText(QString("Actual scale: %1").arg(actualScale, 0, 'f', 3));

    const std::string simulationTimeString = cell::stringutils::timeString(updateTime.count(), 3);
    ui->updateTimeLabel->setText(QString("Update time: %1").arg(QString::fromStdString(simulationTimeString)));

    const std::string simulationUpdateTimeString = cell::stringutils::timeString(simulationUpdateTime.count(), 3);
    ui->simulationTimeLabel->setText(QString("Simulation: %1").arg(QString::fromStdString(simulationUpdateTimeString)));
}