#include "QSFMLCanvas.hpp"

QSFMLCanvas::QSFMLCanvas(QWidget* parent, unsigned int frameTime) : QWidget(parent), initialized_(false)
{
    // Setup some states to allow direct rendering into the widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // Set strong focus to enable keyboard events to be received
    setFocusPolicy(Qt::StrongFocus);

    timer_.setInterval(frameTime);
    resize({851, 1036});
}

void QSFMLCanvas::resizeEvent(QResizeEvent*)
{
    setSize(sf::Vector2u(QWidget::width(), QWidget::height()));
}

QPaintEngine* QSFMLCanvas::paintEngine() const
{
    return 0;
}

void QSFMLCanvas::showEvent(QShowEvent*)
{
    if(initialized_)
        return;

    sf::RenderWindow::create((sf::WindowHandle) winId());
    onInit();

    connect(&timer_, SIGNAL(timeout()), this, SLOT(repaint()));
    timer_.start();

    initialized_ = true;
}

void QSFMLCanvas::paintEvent(QPaintEvent*)
{
    onUpdate();
    display();
}
