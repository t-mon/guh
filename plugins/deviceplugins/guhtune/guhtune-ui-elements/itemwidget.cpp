#include "itemwidget.h"

ItemWidget::ItemWidget(QGraphicsWidget *parent, QColor color) :
    QGraphicsWidget(parent), m_color(color)
{
}

void ItemWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
        painter->setBrush(QBrush(m_color));
        painter->drawEllipse(-rect().width() / 2, -rect().height() /2 ,rect().width(),rect().height());
}
