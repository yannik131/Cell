#ifndef SETUPDIALOG_HPP
#define SETUPDIALOG_HPP

#include "models/SetupModel.hpp"

#include <QDialog>

namespace Ui
{
class SetupDialog;
}

class AbstractSimulationBuilder;
class DiscTypeDistributionTableModel;
class DiscTableModel;

class SetupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SetupDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

    void showEvent(QShowEvent*) override;

private:
    void displayCurrentSetup();

private:
    Ui::SetupDialog* ui;

    DiscTypeDistributionTableModel* discTypeDistributionTableModel_;
    DiscTableModel* discTableModel_;
    SetupModel* setupModel_;
};

#endif /* SETUPDIALOG_HPP */
