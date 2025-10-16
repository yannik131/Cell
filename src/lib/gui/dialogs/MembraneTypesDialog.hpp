#ifndef C8FA243B_5843_4BD9_9E3A_0C6297CED23C_HPP
#define C8FA243B_5843_4BD9_9E3A_0C6297CED23C_HPP

#include <QDialog>

namespace Ui
{
class MembraneTypesDialog;
}

class MembraneTypesTableModel;
class AbstractSimulationBuilder;

class MembraneTypesDialog : public QDialog
{
    Q_OBJECT
public:
    MembraneTypesDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

private:
    void showEvent(QShowEvent* event) override;

private:
    Ui::MembraneTypesDialog* ui;
    MembraneTypesTableModel* membraneTypesTableModel_;
};

#endif /* C8FA243B_5843_4BD9_9E3A_0C6297CED23C_HPP */
