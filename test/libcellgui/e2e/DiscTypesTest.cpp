#include "core/MainWindow.hpp"
#include "dialogs/DiscTypesDialog.hpp"
#include "models/DiscTypesTableModel.hpp"

#include <QApplication>
#include <QPushButton>
#include <QTableView>
#include <QTest>

/**
 * - Add 3 disc types (Type0, Type1, Type2)
 * - Add 2 reactions (Type0 -> Type1, Type0 + Type1 -> Type0 + Type2)
 * - Distribution: Type0: 50%, Type1: 50%
 * - Discs: Type0, Type1, Type2
 * - Try to delete Type0 (not possible, part of distribution)
 * - Delete Type2 (should work, removes reactions and discs)
 * - Rename Type0 to X
 * - Check reactions: X -> Type1
 * - Check distribution: X: 50%, Type1: 50%
 * - Check discs: X, Type1
 */
class DiscTypesTest : public QObject
{
    Q_OBJECT
private slots:
    void test_ui()
    {
        MainWindow mainWindow;
        mainWindow.show();

        QTest::mouseClick(mainWindow.findChild<QPushButton*>("editDiscTypesPushButton"), Qt::LeftButton);

        DiscTypesDialog* discTypesDialog = nullptr;
        // NOLINTNEXTLINE
        QTRY_VERIFY_WITH_TIMEOUT(
            (discTypesDialog = qobject_cast<DiscTypesDialog*>(QApplication::activeModalWidget())) != nullptr, 1000);

        for (int i = 0; i < 3; ++i)
            QTest::mouseClick(discTypesDialog->findChild<QPushButton*>("addTypePushButton"), Qt::LeftButton);

        auto* discTypesTableView = discTypesDialog->findChild<QTableView*>("discTypesTableView");
        auto* discTypesTableModel = qobject_cast<DiscTypesTableModel*>(discTypesTableView->model());

        QVERIFY(discTypesTableModel);
        QCOMPARE(discTypesTableModel->rowCount(), 3);
    }
};

QTEST_MAIN(DiscTypesTest)
#include "DiscTypesTest.moc"