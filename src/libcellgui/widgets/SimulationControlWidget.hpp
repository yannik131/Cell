#ifndef SIMULATIONCONTROLWIDGET_HPP
#define SIMULATIONCONTROLWIDGET_HPP

#include "DiscDistributionPreviewTableModel.hpp"

#include <QMessageBox>
#include <QWidget>

namespace Ui
{
class SimulationControlWidget;
}

class SimulationControlWidget : public QWidget
{
    Q_OBJECT
public:
    SimulationControlWidget(QWidget* parent = nullptr);

signals:
    void simulationStartClicked();
    void simulationStopClicked();
    void simulationResetTriggered();
    void editDiscTypesClicked();
    void editReactionsClicked();

private:
    void setRanges();
    void displayGlobalSettings();
    void setCallbacks();
    void toggleStartStopButtonState();
    void reset();

    template <typename Func> void tryExecuteWithExceptionHandling(Func&& func, QWidget* parent)
    {
        try
        {
            std::forward<Func>(func)();
        }
        catch (const std::exception& e)
        {
            if (auto* self = dynamic_cast<SimulationControlWidget*>(parent))
                self->displayGlobalSettings();

            QMessageBox::critical(parent, "Fehler", e.what());
        }
    }

private:
    Ui::SimulationControlWidget* ui;
    DiscDistributionPreviewTableModel* discDistributionPreviewTableModel_;

    bool simulationStarted_ = false;
};

#endif /* SIMULATIONCONTROLWIDGET_HPP */
