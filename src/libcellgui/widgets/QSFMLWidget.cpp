#include "QSFMLWidget.hpp"

#include <QResizeEvent>

QSFMLWidget::QSFMLWidget(QWidget* parent)
    : QWidget(parent)
{
    // Setup some states to allow direct rendering into the widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // Set strong focus to enable keyboard events to be received
    setFocusPolicy(Qt::StrongFocus);

    setContentsMargins(0, 0, 0, 0);
}

void QSFMLWidget::resizeEvent(QResizeEvent* event)
{
    sf::Vector2f newSize(event->size().width(), event->size().height());
    sf::Vector2f oldSize(event->oldSize().width(), event->oldSize().height());

    RenderWindow::setSize(sf::Vector2u(newSize));
    // The old Views position would now be wrong, simply place a new view with the correct size
    RenderWindow::setView(sf::View(sf::FloatRect(0, 0, newSize.x, newSize.y)));

    QWidget::resizeEvent(event);
}

QPaintEngine* QSFMLWidget::paintEngine() const
{
    return nullptr;
}

void QSFMLWidget::showEvent(QShowEvent*)
{
    if (initialized_)
        return;

    sf::RenderWindow::create((sf::WindowHandle)winId());

    initialized_ = true;
}
