#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTimer>

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

private:
    Ui::MainWindow* ui;
    QTimer retrieveCollisionsTimer_;
    int updateCount_ = 0;
};

#endif /* MAINWINDOW_HPP */
