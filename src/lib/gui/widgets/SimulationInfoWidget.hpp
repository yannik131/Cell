#ifndef A5E68B84_3975_4158_A43A_EF03C2BBEA4B_HPP
#define A5E68B84_3975_4158_A43A_EF03C2BBEA4B_HPP

#include <QWidget>

namespace Ui
{
class SimulationInfoWidget;
}

class SimulationInfoWidget : public QWidget
{
    Q_OBJECT
public:
    SimulationInfoWidget(QWidget* parent = nullptr);
    ~SimulationInfoWidget() override;

    void setRenderData(int targetFPS, int actualFPS, std::chrono::nanoseconds renderTime);
    void setSimulationData(double targetScale, double actualScale, std::chrono::nanoseconds updateTime,
                           std::chrono::nanoseconds simulationUpdateTime);

private:
    std::unique_ptr<Ui::SimulationInfoWidget> ui;
};

#endif /* A5E68B84_3975_4158_A43A_EF03C2BBEA4B_HPP */
