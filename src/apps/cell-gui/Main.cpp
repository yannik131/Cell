#include "core/MainWindow.hpp"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char* argv[])
{
    QApplication::setStyle("fusion");
    QApplication app(argc, argv);

#ifdef DEBUG

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();

#else

    try
    {
        MainWindow mainWindow;
        mainWindow.show();

        return app.exec();
    }
    catch (const std::bad_alloc& exception)
    {
        QMessageBox::critical(nullptr, "Error",
                              QString("Ran out of RAM!\nError description: %1").arg(exception.what()));
    }
    catch (const std::exception& exception)
    {
        QMessageBox::critical(nullptr, "Error",
                              QString("Unhandled exception occured: %1\nThis is probably a bug. Gotta shut down now :(")
                                  .arg(exception.what()));
    }
    catch (...)
    {
        QMessageBox::critical(nullptr, "Error", QString("An unknown exception occured! Closing down."));
    }

#endif
    return 1;
}