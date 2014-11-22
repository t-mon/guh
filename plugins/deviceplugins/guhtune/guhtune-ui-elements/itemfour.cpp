#include "itemfour.h"

ItemFour::ItemFour(QGraphicsWidget *parent) :
    QGraphicsWidget(parent)
{
    m_currentTemperature = 18.5;
}

void ItemFour::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(-rect().width()/2, -rect().height()/2, rect().width(), rect().height(), QPixmap(":/images/template.svg"));

    QColor color(105, 147, 140);

    QPen pen;
    pen.setColor(color);
    pen.setBrush(QBrush(color));
    pen.setWidth(5);
    painter->setPen(pen);

    painter->drawRect(rect().width()/10 + rect().width()/20, rect().height()/3, -rect().width()/10, -rect().height()/1.5);
    painter->drawRect(-rect().width()/10 - rect().width()/20, rect().height()/3, rect().width()/10, -rect().height()/1.5);

    painter->drawText(QRectF(rect().width()/10 + rect().width()/20 + rect().width()/10, 0, rect().width()/8, rect().width()/8), QString::number(m_currentTemperature)+ "°");

}

void ItemFour::update()
{

}
