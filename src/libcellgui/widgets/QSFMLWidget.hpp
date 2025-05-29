#ifndef QSFMLWIDGET_HPP
#define QSFMLWIDGET_HPP

#include <QTimer>
#include <QWidget>
#include <SFML/Graphics/RenderWindow.hpp>

class QSFMLWidget : public QWidget, public sf::RenderWindow
{
    Q_OBJECT
public:
    QSFMLWidget(QWidget* parent);

    ~QSFMLWidget() override = default;
    void resizeEvent(QResizeEvent* event) override;

private:
    QPaintEngine* paintEngine() const override;
    void showEvent(QShowEvent*) override;

private:
    bool initialized_{false};
};

#endif /* QSFMLWIDGET_HPP */
