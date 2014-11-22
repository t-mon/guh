#include "itemwidget.h"

ItemWidget::ItemWidget(QGraphicsWidget *parent, QColor color, int number) :
    QGraphicsWidget(parent), m_color(color), m_number(number)
{
}

void ItemWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
        painter->setBrush(QBrush(m_color));
        painter->drawEllipse(-rect().width() / 2, -rect().height() /2 , rect().width(), rect().height());
        painter->setPen(QPen(Qt::black));
        painter->drawText(0, 0, QString::number(m_number));
}
