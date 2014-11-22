#ifndef GUHTUNEUI_H
#define GUHTUNEUI_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QStateMachine>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QAbstractTransition>
#include <QSequentialAnimationGroup>

#include "guhtune-ui-elements/clock.h"
#include "guhtune-ui-elements/itemwidget.h"

class GuhTuneUi : public QMainWindow
{
    Q_OBJECT

public:
    explicit GuhTuneUi(QWidget *parent = 0);
    ~GuhTuneUi();

private:
    //QStateMachine *m_machine;

    QGraphicsScene *m_scene;
    QGraphicsView *m_view;

    QGraphicsPixmapItem *m_splashItem;
    Clock *m_clock;

    QState *createGeometryState(QObject *w1, const QRect &rect1,
                                QObject *w2, const QRect &rect2,
                                QObject *w3, const QRect &rect3,
                                QObject *w4, const QRect &rect4,
                                QState *parent);

protected:
     void keyPressEvent(QKeyEvent *keyEvent);

signals:
    // Called from ui
    void toggle(int index);
    void setValue(int index, int value);

    void navigationLeft();
    void navigationRight();

    // Called from plugin code
    void buttonPressed();
    void buttonReleased();
    void wakeup();

};

#endif // GUHTUNEUI_H
