#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include <QGraphicsWidget>
#include <QDebug>
#include <QPainter>

class ItemWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit ItemWidget(QGraphicsWidget *parent = 0, QColor color = QColor(), int number = 0);

private:
    QColor m_color;
    int m_number;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

};

#endif // ITEMWIDGET_H
