#ifndef FF484939_FF4B_43EC_AAEA_E6416E44A6F1_HPP
#define FF484939_FF4B_43EC_AAEA_E6416E44A6F1_HPP

#include <QDialog>

namespace Ui
{
class ReactionsDialog;
};

class ReactionsTableModel;
class SimulationConfigUpdater;

/**
 * @brief Dialog where the user can create/delete reactions
 */
class ReactionsDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief Creates the delegates for displaying the reactions and connects callbacks
     */
    explicit ReactionsDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void showEvent(QShowEvent* event) override;

private:
    Ui::ReactionsDialog* ui;
    ReactionsTableModel* reactionsTableModel_;
};

#endif /* FF484939_FF4B_43EC_AAEA_E6416E44A6F1_HPP */
