#include "SetupDialog.hpp"

#include "ui_SetupDialog.h"

SetupDialog::SetupDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QDialog(parent)
    , ui(new Ui::SetupDialog)
{
}