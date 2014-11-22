#ifndef ITEMONE_H
#define ITEMONE_H

#include <QGraphicsWidget>
#include <QPainter>
#include <QDebug>

class ItemOne : public QGraphicsWidget
{
public:
    ItemOne(QGraphicsWidget *parent = 0);
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

#endif // ITEMONE_H
