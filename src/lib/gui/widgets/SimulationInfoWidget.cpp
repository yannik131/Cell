#include "widgets/SimulationInfoWidget.hpp"
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
    ui->targetFPSLabel->setText(QString("TFPS: %1").arg(targetFPS));
    ui->actualFPSLabel->setText(QString("FPS: %1").arg(actualFPS));

    const std::string renderTimeString = cell::stringutils::timeString(renderTime.count(), 3);
    ui->renderTimeLabel->setText(QString("t: %1").arg(QString::fromStdString(renderTimeString)));
}

void SimulationInfoWidget::setPerformanceData(const cell::SimulationRunner::PerformanceData& performanceData)
{
    ui->targetScaleLabel->setText(QString("Tscale: %1").arg(performanceData.targetScale, 0, 'f', 3));
    ui->actualScaleLabel->setText(QString("Ascale: %1").arg(performanceData.actualScale, 0, 'f', 3));

    const std::string simulationTimeString = cell::stringutils::timeString(
        ch::duration_cast<ch::nanoseconds>(performanceData.timePerWholeUpdate).count(), 3);
    ui->updateTimeLabel->setText(QString("t: %1").arg(QString::fromStdString(simulationTimeString)));

    const std::string simulationUpdateTimeString = cell::stringutils::timeString(
        ch::duration_cast<ch::nanoseconds>(performanceData.timePerSimulationUpdate).count(), 3);
    ui->simulationTimeLabel->setText(QString("t_S: %1").arg(QString::fromStdString(simulationUpdateTimeString)));
}