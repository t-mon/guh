#ifndef ITEMTHREE_H
#define ITEMTHREE_H

#include <QGraphicsWidget>
#include <QPainter>
#include <QDebug>

class ItemThree : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit ItemThree(QGraphicsWidget *parent = 0);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

signals:

public slots:

};

#endif // ITEMTHREE_H
