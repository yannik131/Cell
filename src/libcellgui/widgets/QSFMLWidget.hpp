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

    virtual ~QSFMLWidget() = default;
    void resizeEvent(QResizeEvent* event);

private:
    virtual QPaintEngine* paintEngine() const;
    virtual void showEvent(QShowEvent*);

private:
    bool initialized_;
};

#endif /* QSFMLWIDGET_HPP */
