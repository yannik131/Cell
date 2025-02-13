#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "FrameDTO.hpp"
#include "Simulation.hpp"

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

private:
    void startSimulation();

private:
    Ui::MainWindow* ui;
    QThread *simulationThread_;
    Simulation* simulation_;

    const QString StartString = "Start";
    const QString StopString = "Stop";
};

#endif /* MAINWINDOW_HPP */
