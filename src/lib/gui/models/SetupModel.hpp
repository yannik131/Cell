#ifndef SETUPMODEL_HPP
#define SETUPMODEL_HPP

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
    void setCellWidth(int cellWidth);
    void setCellHeight(int cellHeight);
    void setGUIFPS(int GUIFPS);

    // TODO make an interface out of this, there's 3 dialogs and models doing the same shit
    void commitChanges();
    void discardChanges();
    const cell::config::Setup& getSetup();

private:
    void loadDiscsAndDistribution();

private:
    AbstractSimulationBuilder* abstractSimulationBuilder_;
    DiscTypeDistributionTableModel* discTypeDistributionTableModel_;
    DiscTableModel* discTableModel_;

    cell::config::Setup setup_;
};

#endif /* SETUPMODEL_HPP */
