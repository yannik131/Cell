#ifndef A9DE5EDF_968A_4196_840D_F282E2521335_HPP
#define A9DE5EDF_968A_4196_840D_F282E2521335_HPP

#include <QDialog>

namespace Ui
{
class DiscTypeDistributionDialog;
};

class DiscTypeDistributionTableModel;
class AbstractSimulationBuilder;

class DiscTypeDistributionDialog : public QDialog
{
    Q_OBJECT
public:
    DiscTypeDistributionDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

private:
    void showEvent(QShowEvent* event) override;

private:
    Ui::DiscTypeDistributionDialog* ui;
    DiscTypeDistributionTableModel* discTypeDistributionTableModel_;
};

#endif /* A9DE5EDF_968A_4196_840D_F282E2521335_HPP */
