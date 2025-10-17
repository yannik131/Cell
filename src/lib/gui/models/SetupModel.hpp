#ifndef CA3C5220_84D1_4F43_AEE6_80E28349C546_HPP
#define CA3C5220_84D1_4F43_AEE6_80E28349C546_HPP

#include "cell/SimulationConfig.hpp"

#include <QObject>

class DiscTypeDistributionTableModel;
class DiscTableModel;
class AbstractSimulationBuilder;

class SetupModel : public QObject
{
    Q_OBJECT
public:
    explicit SetupModel(QObject* parent, DiscTypeDistributionTableModel* discTypeDistributionTableModel,
                        DiscTableModel* discTableModel, AbstractSimulationBuilder* abstractSimulationBuilder);

    void setUseDistribution(bool useDistribution);
    void setNumberOfDiscs(int numberOfDiscs);
    void setTimeStepUs(int timeStepUs);
    void setTimeScale(double timeScale);
    void setCellRadius(int radius);
    void setMaxVelocity(int maxVelocity);

    // TODO make an interface out of this, there's 3 dialogs and models doing the same shit
    void commitChanges();
    void reload();
    const cell::config::Setup& getSetup();

private:
    void loadDiscsAndDistribution();

private:
    AbstractSimulationBuilder* abstractSimulationBuilder_;
    DiscTypeDistributionTableModel* discTypeDistributionTableModel_;
    DiscTableModel* discTableModel_;

    cell::config::Setup setup_;
};

#endif /* CA3C5220_84D1_4F43_AEE6_80E28349C546_HPP */
