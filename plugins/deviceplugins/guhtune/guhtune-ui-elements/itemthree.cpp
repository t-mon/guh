#include "itemthree.h"

ItemThree::ItemThree(QGraphicsWidget *parent) :
    QGraphicsWidget(parent)
{
}

void ItemThree::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(-rect().width()/2, -rect().height()/2, rect().width(), rect().height(), QPixmap(":/images/light.svg"));

}
