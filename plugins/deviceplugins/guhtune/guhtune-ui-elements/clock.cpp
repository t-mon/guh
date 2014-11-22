#include "clock.h"

Clock::Clock(QGraphicsWidget *parent) :
    QGraphicsWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeClick()));
    timer->start(1000);
}

void Clock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    static const QPoint hourHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -50)
    };
    static const QPoint minuteHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -80)
    };

    QColor hourColor(105, 147, 140);
    QColor minuteColor(140, 193, 182, 190);

    QPen pen;
    pen.setWidth(4);
    pen.setColor(hourColor);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::black);
    painter->drawEllipse(QRect(0,0, rect().width(), rect().height()));


    int side = qMin(rect().width(), rect().height());
    QTime time = QTime::currentTime();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->translate(rect().width() / 2, rect().height() / 2);
    painter->scale(side / 200.0, side / 200.0);

    painter->setPen(Qt::NoPen);
    painter->setBrush(hourColor);

    painter->save();
    painter->rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    painter->drawConvexPolygon(hourHand, 3);
    painter->restore();

    painter->setPen(hourColor);

    for (int i = 0; i < 12; ++i) {
        painter->drawLine(88, 0, 96, 0);
        painter->rotate(30.0);
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(minuteColor);

    painter->save();
    painter->rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter->drawConvexPolygon(minuteHand, 3);
    painter->restore();

    painter->setPen(minuteColor);

    for (int j = 0; j < 60; ++j) {
        if ((j % 5) != 0)
            painter->drawLine(92, 0, 96, 0);
        painter->rotate(6.0);
    }
}

void Clock::timeClick()
{
    emit clockChanged();
    update();
}
