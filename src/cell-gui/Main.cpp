#include "GlobalGUISettings.hpp"
#include "GlobalSettings.hpp"
#include "GlobalSettingsFunctor.hpp"
#include "Logging.hpp"
#include "MainWindow.hpp"
#include "Settings.hpp"

#include <QApplication>
#include <QMessageBox>
#include <QtCore/QMetaType>

int main(int argc, char* argv[])
{
    cell::initLogging(argc, argv);
    QApplication app(argc, argv);

    auto& functor = GlobalSettingsFunctor::get();
    cell::GlobalSettings::setCallback([&functor](const cell::SettingID& settingID) { functor(settingID); });

#ifndef DEBUG
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

#else

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();

#endif
    return 1;
}