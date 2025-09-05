#ifndef SETUPDIALOG_HPP
#define SETUPDIALOG_HPP

#include <QDialog>

namespace Ui
{
class SetupDialog;
}

class AbstractSimulationBuilder;

class SetupDialog
{
    Q_OBJECT
public:
    explicit SetupDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

private:
    Ui::SetupDialog* ui;
};

#endif /* SETUPDIALOG_HPP */
