#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <QWidget>
#include <SFML/Graphics.hpp>
#include <QTimer>

class QSFMLCanvas : public QWidget, public sf::RenderWindow
{
    Q_OBJECT
public:
    explicit QSFMLCanvas(QWidget *parent = 0);
    virtual  ~QSFMLCanvas();

    virtual void OnInit();

    virtual void OnUpdate();

    virtual QPaintEngine *paintEngine() const;

    virtual void showEvent(QShowEvent*);

    virtual void paintEvent(QPaintEvent*);

    QTimer myTimer;
    bool   myInitialized;

signals:


public slots:
};

#endif // QSFMLCANVAS_H
