#ifndef CA3C5220_84D1_4F43_AEE6_80E28349C546_HPP
#define CA3C5220_84D1_4F43_AEE6_80E28349C546_HPP

#include "cell/SimulationConfig.hpp"
#include "models/AbstractSimulationConfigTableModel.hpp"

#include <QObject>

class SimulationConfigUpdater;

class SetupModel : public QObject, public AbstractConfigChanger
{
    Q_OBJECT
public:
    SetupModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void setUseDistribution(bool useDistribution);
    void setTimeStepUs(int timeStepUs);
    void setTimeScale(double timeScale);
    void setMaxVelocity(int maxVelocity);

    void saveToConfig();
    void loadFromConfig();

private:
    SimulationConfigUpdater* simulationConfigUpdater_;
    cell::SimulationConfig simulationConfig_;
};

#endif /* CA3C5220_84D1_4F43_AEE6_80E28349C546_HPP */
