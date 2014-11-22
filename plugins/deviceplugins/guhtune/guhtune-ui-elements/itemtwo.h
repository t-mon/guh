#ifndef ITEMTWO_H
#define ITEMTWO_H

#include <QGraphicsWidget>
#include <QPainter>
#include <QDebug>

class ItemTwo : public QGraphicsWidget
{
public:
    ItemTwo(QGraphicsWidget *parent = 0);
    int value();
    void tickLeft();
    void tickRight();

private:
    QColor m_color;
    int m_value;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

signals:
    void valueChaged(const int &item, const int &value);
};

#endif // ITEMTWO_H
