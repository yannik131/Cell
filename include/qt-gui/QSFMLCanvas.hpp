#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <QWidget>
#include <QTimer>

class QSFMLCanvas : public QWidget, public sf::RenderWindow
{
public:
    QSFMLCanvas(QWidget* parent, unsigned int frameTime = 5);

    virtual ~QSFMLCanvas() = default;
    void resizeEvent(QResizeEvent* event);

private:
    virtual void onInit() = 0;
    virtual void onUpdate() = 0;
    virtual QPaintEngine* paintEngine() const;
    virtual void showEvent(QShowEvent*);
    virtual void paintEvent(QPaintEvent*);


private:
    QTimer timer_;
    bool initialized_;
};


#endif /* QSFMLCANVAS_H */
