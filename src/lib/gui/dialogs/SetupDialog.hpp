#ifndef C4E43CD6_7DB6_4EBF_8134_6C676407AECA_HPP
#define C4E43CD6_7DB6_4EBF_8134_6C676407AECA_HPP

#include "models/SetupModel.hpp"

#include <QDialog>

namespace Ui
{
class SetupDialog;
}

class SimulationConfigUpdater;
class DiscTypeDistributionTableModel;
class DiscTableModel;

class SetupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SetupDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void showEvent(QShowEvent*) override;

private:
    void displayCurrentSetup();

private:
    Ui::SetupDialog* ui;

    DiscTypeDistributionTableModel* discTypeDistributionTableModel_;
    DiscTableModel* discTableModel_;
    SetupModel* setupModel_;
};

#endif /* C4E43CD6_7DB6_4EBF_8134_6C676407AECA_HPP */
