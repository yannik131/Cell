#include "QSFMLWidget.hpp"

QSFMLWidget::QSFMLWidget(QWidget* parent) : QWidget(parent), initialized_(false)
{
    // Setup some states to allow direct rendering into the widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // Set strong focus to enable keyboard events to be received
    setFocusPolicy(Qt::StrongFocus);

    resize({851, 1036});
    setSize(sf::Vector2u(851, 1036));
}

void QSFMLWidget::resizeEvent(QResizeEvent*)
{
    setSize(sf::Vector2u(QWidget::width(), QWidget::height()));
}

QPaintEngine* QSFMLWidget::paintEngine() const
{
    return 0;
}

void QSFMLWidget::showEvent(QShowEvent*)
{
    if(initialized_)
        return;

    sf::RenderWindow::create((sf::WindowHandle) winId());

    initialized_ = true;
}
