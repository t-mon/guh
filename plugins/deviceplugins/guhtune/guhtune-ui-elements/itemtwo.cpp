#include "itemtwo.h"

ItemTwo::ItemTwo(QGraphicsWidget *parent):
    QGraphicsWidget(parent)
{
    m_value = 60;
}

int ItemTwo::value()
{
    return m_value;
}

void ItemTwo::tickLeft()
{
    if (m_value <= 0) {
        return;
    }
    m_value -= 2;
    if (m_value <= 0) {
        m_value = 0;
    }
    //emit valueChaged(1,m_value);
    qDebug() << "item 2 value" << m_value;
    update();
}

void ItemTwo::tickRight()
{
    if (m_value >= 100) {
        return;
    }
    m_value += 2;
    if (m_value > 100) {
        m_value = 100;
    }
    //emit valueChaged(1,m_value);
    qDebug() << "item 2 value" << m_value;
    update();
}



void ItemTwo::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(-rect().width()/2, -rect().height()/2, rect().width(), rect().height(), QPixmap(":/images/work.svg"));

    QColor patternColor(205, 226, 216);
    QColor valueColor(105, 147, 140);

    QPen pen;
    pen.setWidth(rect().width() / 103);
    pen.setBrush(Qt::black);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    // draw values
    pen.setBrush(valueColor);
    painter->setPen(pen);
    for (int i = 0; i < m_value; ++i) {
        painter->drawLine( (rect().height()/2 - rect().width() / 22)*sin(-30*0.0174532925) , (rect().height()/2 - rect().width() / 22) *cos(-30*0.0174532925) , (rect().height() / 2 - rect().width() / 10)*sin(-30*0.0174532925), (rect().height() / 2 - rect().width() / 10)*cos(-30*0.0174532925));
        painter->rotate(3);
    }

    // draw pattern
    pen.setBrush(patternColor);
    painter->setPen(pen);
    for (int i = 1; i <= (100-m_value); ++i) {
        painter->drawLine( (rect().height()/2 - rect().width() / 22)*sin(-30*0.0174532925) , (rect().height()/2 - rect().width() / 22) *cos(-30*0.0174532925) , (rect().height() / 2 - rect().width() / 10)*sin(-30*0.0174532925), (rect().height() / 2 - rect().width() / 10)*cos(-30*0.0174532925));
        painter->rotate(3);
    }
}
