#ifndef C4E43CD6_7DB6_4EBF_8134_6C676407AECA_HPP
#define C4E43CD6_7DB6_4EBF_8134_6C676407AECA_HPP

#include <QDialog>

namespace Ui
{
class SetupDialog;
}

class SimulationConfigUpdater;
class SetupModel;

class SetupDialog : public QDialog
{
    Q_OBJECT
public:
    SetupDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);
    ~SetupDialog() override;

    void showEvent(QShowEvent*) override;

    void displayCurrentConfig();

private:
    std::unique_ptr<Ui::SetupDialog> ui;
    SetupModel* setupModel_;
    SimulationConfigUpdater* simulationConfigUpdater_;
};

#endif /* C4E43CD6_7DB6_4EBF_8134_6C676407AECA_HPP */
