#ifndef C4E43CD6_7DB6_4EBF_8134_6C676407AECA_HPP
#define C4E43CD6_7DB6_4EBF_8134_6C676407AECA_HPP

#include <QDialog>

namespace Ui
{
class SetupDialog;
}

class SimulationConfigUpdater;
class DiscTableModel;
class MembranesTableModel;
class SetupModel;

class SetupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SetupDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void showEvent(QShowEvent*) override;

private:
    Ui::SetupDialog* ui;

    DiscTableModel* discTableModel_;
    MembranesTableModel* membranesTableModel_;
    SetupModel* setupModel_;
};

#endif /* C4E43CD6_7DB6_4EBF_8134_6C676407AECA_HPP */
