#ifndef FEF60E3C_E40A_4709_A34D_F063F099C6DB_HPP
#define FEF60E3C_E40A_4709_A34D_F063F099C6DB_HPP

#include "core/Types.hpp"

#include <QTimer>
#include <QWidget>
#include <SFML/Graphics/RenderWindow.hpp>

/**
 * @brief Widget containing a `sf::RenderWindow` for drawing things using SFML. We use this over a `QGraphicsView`
 * because I don't think it's performant enough to display several thousand circles with hundreds of FPS
 */
class QSFMLWidget : public QWidget, public sf::RenderWindow
{
    Q_OBJECT
private:
    enum class ZoomDirection
    {
        In,
        Out
    };

public:
    /**
     * @brief See https://github.com/SFML/SFML/wiki/Tutorial:-Integrating-SFML-into-Qt for additional information on
     * what happens here
     */
    QSFMLWidget(QWidget* parent);

    /**
     * @brief After resizing the widget, a new `sf::View` has to be created, otherwise part of the render window won't
     * be visible anymore
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief Starts dragging by setting the initial position
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief Drags the world according to the mouse offset, taking zoom into account
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief Stops dragging
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /**
     * @brief Increases/Decreases the zoom
     */
    void wheelEvent(QWheelEvent* event) override;

    /**
     * @brief Zoom using + and - keys
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief Creates a new view without zoom and translation and resets zoom and translation offset
     */
    void resetView(Zoom zoom = Zoom{1});

    /**
     * @returns the accumulative zoom of the view
     */
    double getCurrentZoom() const;

    void setMinimumSize(const QSize& s);

signals:
    void renderRequired();

private:
    /**
     * @brief No paint engine is used here since we use SFML directly for drawing
     */
    QPaintEngine* paintEngine() const override;

    /**
     * @brief Creates the `sf::RenderWindow` using the existing window handle
     */
    void showEvent(QShowEvent*) override;

    void zoom(ZoomDirection direction);

private:
    bool initialized_ = false;
    sf::View view_;
    QPoint offset_;
    QPoint startPosition_;
    bool dragging_ = false;
    double currentZoom_ = 1;
};

#endif /* FEF60E3C_E40A_4709_A34D_F063F099C6DB_HPP */
