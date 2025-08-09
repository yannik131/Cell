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
    sf::Vector2u newSize(event->size().width(), event->size().height());
    sf::Vector2u oldSize(event->oldSize().width(), event->oldSize().height());

    RenderWindow::setSize(newSize);
    // The old Views position would now be wrong, simply place a new view with the correct size
    view_ = sf::View(sf::FloatRect(0, 0, static_cast<float>(newSize.x), static_cast<float>(newSize.y)));
    view_.move(static_cast<float>(offset_.x()), static_cast<float>(offset_.y()));
    view_.zoom(static_cast<float>(currentZoom_));

    RenderWindow::setView(view_);
    QWidget::resizeEvent(event);
}

void QSFMLWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        startPosition_ = event->pos();
        dragging_ = true;
    }
}

void QSFMLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!dragging_)
        return;

    QPointF delta = event->pos() - startPosition_;
    startPosition_ = event->pos();

    sf::Vector2i sfDelta(static_cast<int>(delta.x()), static_cast<int>(delta.y()));

    // The view could have zoom, so we need to take that into account by scaling the delta appropriately
    sf::Vector2f worldDelta =
        RenderWindow::mapPixelToCoords(sf::Vector2i(0, 0)) - RenderWindow::mapPixelToCoords(sfDelta);

    view_.move(worldDelta);
    RenderWindow::setView(view_);

    offset_ += QPoint(static_cast<int>(worldDelta.x), static_cast<int>(worldDelta.y));

    emit renderRequired();
}

void QSFMLWidget::mouseReleaseEvent(QMouseEvent*)
{
    if (dragging_)
        dragging_ = false;
}

void QSFMLWidget::wheelEvent(QWheelEvent* event)
{
    auto delta = event->angleDelta().y();

    float zoomFactor = (delta > 0) ? 0.9f : 1.1f;
    currentZoom_ *= zoomFactor;
    view_.zoom(zoomFactor);

    RenderWindow::setView(view_);

    emit renderRequired();
}

void QSFMLWidget::resetView()
{
    view_ = sf::View(
        sf::FloatRect(0, 0, static_cast<float>(QWidget::size().width()), static_cast<float>(QWidget::size().height())));
    currentZoom_ = 1;
    offset_ = QPoint(0, 0);

    RenderWindow::setView(view_);

    emit renderRequired();
}

double QSFMLWidget::getCurrentZoom() const
{
    return currentZoom_;
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
