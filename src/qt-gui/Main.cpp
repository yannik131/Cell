#include <QApplication>
#include <QWidget>

#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Simple Qt Window");
    window.resize(400, 300);
    window.show();

    return app.exec();
}