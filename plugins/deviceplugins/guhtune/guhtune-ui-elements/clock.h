#ifndef CLOCK_H
#define CLOCK_H

#include <QGraphicsWidget>
#include <QTimer>
#include <QTime>
#include <QPainter>

class Clock : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit Clock(QGraphicsWidget *parent = 0);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

signals:
    void clockChanged();

private slots:
    void timeClick();

};

#endif // CLOCK_H
