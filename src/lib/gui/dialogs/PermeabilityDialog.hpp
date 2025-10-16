#ifndef DBACDD6F_AD4A_44E4_993F_82747E3C5E8C_HPP
#define DBACDD6F_AD4A_44E4_993F_82747E3C5E8C_HPP

#include <QDialog>

namespace Ui
{
class PermeabilityDialog;
};

class PermeabilityTableModel;
class AbstractSimulationBuilder;

class PermeabilityDialog : public QDialog
{
    Q_OBJECT
public:
    PermeabilityDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder,
                       PermeabilityTableModel* permeabilityTableModel);

private:
    void showEvent(QShowEvent* event) override;

private:
    Ui::PermeabilityDialog* ui;
    PermeabilityTableModel* permeabilityTableModel_;
};

#endif /* DBACDD6F_AD4A_44E4_993F_82747E3C5E8C_HPP */
