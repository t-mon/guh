#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include <QGraphicsWidget>
#include <QPainter>

class ItemWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit ItemWidget(QGraphicsWidget *parent = 0, QColor color = QColor());

private:
    QColor m_color;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

};

#endif // ITEMWIDGET_H
