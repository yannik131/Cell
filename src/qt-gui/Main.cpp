#include "Logging.hpp"
#include "MainWindow.hpp"
#include "Settings.hpp"

#include <QApplication>
#include <QMessageBox>
#include <QtCore/QMetaType>

Q_DECLARE_METATYPE(Settings);

int main(int argc, char* argv[])
{
    initLogging(argc, argv);
    QApplication app(argc, argv);

    try
    {
        MainWindow mainWindow;
        mainWindow.show();

        return app.exec();
    }

    catch (const std::exception& e)
    {
        QMessageBox::critical(
            nullptr, "Error",
            QString("Unhandled exception occured: %1\nThis is probably a bug. Gotta shut down now :(").arg(e.what()));
    }
    catch (...)
    {
        QMessageBox::critical(nullptr, "Error", QString("An unknown exception occured! Closing down."));
    }

    return 1;
}