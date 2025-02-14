#include "QSFMLWidget.hpp"

#include <QResizeEvent>

QSFMLWidget::QSFMLWidget(QWidget* parent) : QWidget(parent), initialized_(false)
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
    RenderWindow::setSize(sf::Vector2u(event->size().width(), event->size().height()));
    sf::View view = RenderWindow::getDefaultView();
    view.setSize({
            static_cast<float>(event->size().width()),
            static_cast<float>(event->size().height())
    });
    RenderWindow::setView(view);

    QWidget::resizeEvent(event);
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
