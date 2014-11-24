#include "itemfour.h"

ItemFour::ItemFour(QGraphicsWidget *parent) :
    QGraphicsWidget(parent)
{
    m_currentTemperature = 18.5;
    m_heatingTemperature = 18.5;

}

void ItemFour::tickLeft()
{
    if (m_heatingTemperature == 0) {
        return;
    }
    m_heatingTemperature -= 0.5;
    qDebug() << "heating set to " << m_heatingTemperature;
    update();
}

void ItemFour::tickRight()
{
    if (m_heatingTemperature == 30) {
        return;
    }
    m_heatingTemperature += 0.5;
    qDebug() << "heating set to " << m_heatingTemperature;
    update();
}

void ItemFour::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(-rect().width()/2, -rect().height()/2, rect().width(), rect().height(), QPixmap(":/images/radiator.svg"));

    QColor color(105, 147, 140);

    QPen pen;
    pen.setColor(color);
    pen.setBrush(QBrush(color));

    // base Line
    pen.setWidth(rect().width()/100);
    painter->setPen(pen);
    painter->drawLine(QPoint(-rect().width()/5, rect().width()/5 ), QPoint(rect().width()/5, rect().width()/5));


    // heating temp
    pen.setWidth(rect().width()/10);
    painter->setPen(pen);
    QPoint startHeating(rect().width()/10, rect().width()/6.5);
    QPoint endHeating  (rect().width()/10, rect().width()/6.5 - (rect().width()/ 60 * m_heatingTemperature));
    painter->drawLine(startHeating, endHeating);

    // heating temp line
    pen.setWidth(rect().width()/120);
    painter->setPen(pen);
    //QPoint startHeatingTempLine(endHeating + QPoint(0, -rect().width()/21.5));
    QPoint endHeatingTempLine(endHeating + QPoint(rect().width()/10, -rect().width()/21.5));
    //painter->drawLine(startHeatingTempLine, endHeatingTempLine);

    // heating temp text
    QRect heatingTempText(QPoint(endHeatingTempLine + QPoint(rect().width()/30, 0)).rx(), QPoint(endHeatingTempLine + QPoint(rect().width()/30, 0)).ry(), rect().width()/30, rect().width()/30 );
    painter->drawText(heatingTempText, QString::number(m_heatingTemperature) + "°C");


    // current temp
    pen.setWidth(rect().width()/10);
    painter->setPen(pen);
    QPoint startCurrent(-rect().width()/10, rect().width()/6.5);
    QPoint endCurrent(-rect().width()/10, rect().width()/6.5 - (rect().width()/ 60 * m_currentTemperature));
    painter->drawLine(startCurrent, endCurrent);

}

void ItemFour::update()
{

}
