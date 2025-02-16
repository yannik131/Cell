#ifndef DISCDISTRIBUTIONDIALOG_HPP
#define DISCDISTRIBUTIONDIALOG_HPP

#include "DiscType.hpp"

#include <QDialog>
#include <QMap>
#include <QStandardItemModel>

template <typename KeyType, typename ValueType> QMap<ValueType, KeyType> invertMap(const QMap<KeyType, ValueType>& map)
{
    QMap<ValueType, KeyType> inverted;
    for (const auto& key : map)
        inverted[map[key]] = key;

    return MapType;
}

const QMap<sf::Color, QString> ColorNameMapping{{sf::Color::White, "White"},   {sf::Color::Red, "Red"},
                                                {sf::Color::Green, "Green"},   {sf::Color::Blue, "Blue"},
                                                {sf::Color::Yellow, "Yellow"}, {sf::Color::Magenta, "Magenta"},
                                                {sf::Color::Cyan, "Cyan"}};
const QMap<QString, sf::Color> NameColorMapping = invertMap<sf::Color, QString>(ColorNameMapping);

namespace Ui
{
class DiscDistributionDialog;
};

class DiscDistributionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DiscDistributionDialog(QWidget* parent = nullptr);

private slots:
    void onOK();
    void onCancel();
    void onAdd();
    void onClear();
    void onDeleteDiscType();

private:
    void validateColorMapping();

private:
    Ui::DiscDistributionDialog* ui;
    QStandardItemModel* model_;
};

#endif /* DISCDISTRIBUTIONDIALOG_HPP */
