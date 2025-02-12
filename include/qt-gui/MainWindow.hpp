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

    void updatePlot();

private slots:
    void onCollisionData(int collisions);

private:
    Ui::MainWindow* ui;
    int updateCount_ = 0;
    QThread *simulationThread_;
    Simulation* simulation_;
};

#endif /* MAINWINDOW_HPP */
