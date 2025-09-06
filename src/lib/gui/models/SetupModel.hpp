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

    // TODO make an interface out of this, there's 3 dialogs and models doing the same shit
    void commitChanges();
    void discardChanges();

private:
    AbstractSimulationBuilder* abstractSimulationBuilder_;
    DiscTypeDistributionTableModel* discTypeDistributionTableModel_;
    DiscTableModel* discTableModel_;

    cell::config::Setup setup_;
};

#endif /* SETUPMODEL_HPP */
