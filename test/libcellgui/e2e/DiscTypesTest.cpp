#include "core/MainWindow.hpp"
#include "dialogs/DiscTypesDialog.hpp"
#include "models/DiscTypesTableModel.hpp"

#include <QApplication>
#include <QPushButton>
#include <QTableView>
#include <QTest>

class DiscTypesTest : public QObject
{
    Q_OBJECT
private slots:
    void test_ui()
    {
        MainWindow mainWindow;
        mainWindow.show();

        auto* editDiscTypesPushButton = mainWindow.findChild<QPushButton*>("editDiscTypesPushButton");

        QVERIFY(editDiscTypesPushButton);

        QTest::mouseClick(editDiscTypesPushButton, Qt::LeftButton);

        DiscTypesDialog* discTypesDialog = nullptr;
        QTRY_VERIFY_WITH_TIMEOUT(
            (discTypesDialog = qobject_cast<DiscTypesDialog*>(QApplication::activeModalWidget())) != nullptr, 1000);

        auto* addTypePushButton = discTypesDialog->findChild<QPushButton*>("addTypePushButton");
        QVERIFY(addTypePushButton);

        QTest::mouseClick(addTypePushButton, Qt::LeftButton);

        auto* discTypesTableView = discTypesDialog->findChild<QTableView*>("discTypesTableView");
        QVERIFY(discTypesTableView);

        auto* discTypesTableModel = qobject_cast<DiscTypesTableModel*>(discTypesTableView->model());
        QVERIFY(discTypesTableModel);

        QCOMPARE(discTypesTableModel->rowCount(), 1);
    }
};

QTEST_MAIN(DiscTypesTest)
#include "DiscTypesTest.moc"