#ifndef ITEMFOUR_H
#define ITEMFOUR_H

#include <QGraphicsWidget>
#include <QPainter>
#include <QDebug>
#include <QTimer>

class ItemFour : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit ItemFour(QGraphicsWidget *parent = 0);
    void tickLeft();
    void tickRight();

private:
    double m_currentTemperature;
    double m_heatingTemperature;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

signals:

private slots:
    void update();
};

#endif // ITEMFOUR_H
