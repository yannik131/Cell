#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "FrameDTO.hpp"
#include "Simulation.hpp"
#include "SimulationSettings.hpp"

#include <QMainWindow>
#include <QTimer>
#include <QThread>

namespace Ui 
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT 
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

    void onStartStopButtonClicked();
    void onResetButtonClicked();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void startSimulation();
    void stopSimulation();

private:
    Ui::MainWindow* ui;
    QThread *simulationThread_;
    Simulation* simulation_;

    const QString StartString = "Start";
    const QString StopString = "Stop";

    bool initialSizeSet_ = false;
};

#endif /* MAINWINDOW_HPP */
