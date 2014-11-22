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
#include "guhtune-ui-elements/itemone.h"
#include "guhtune-ui-elements/itemtwo.h"

class GuhTuneUi : public QMainWindow
{
    Q_OBJECT

public:
    explicit GuhTuneUi(QWidget *parent = 0);
    ~GuhTuneUi();

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;

    QGraphicsPixmapItem *m_splashItem;
    Clock *m_clock;
    ItemOne *m_itemOne;
    ItemTwo *m_itemTwo;
    ItemWidget *m_itemThree;
    ItemWidget *m_itemFour;

    QStateMachine *m_machine;
    QState *m_state1;
    QState *m_state2;
    QState *m_state3;
    QState *m_state4;
    QState *m_state1FullScreen;
    QState *m_state2FullScreen;
    QState *m_state3FullScreen;
    QState *m_state4FullScreen;

    QState *m_currentState;

protected:
    void keyPressEvent(QKeyEvent *keyEvent);

signals:
    void navigationLeft();
    void navigationRight();
    void enterOverview();
    void exitOverview();

    void itemPressed(int itemNumber);
    void itemValueChanged(int itemNumber, int value);

private slots:
    void onState1();
    void onState2();
    void onState3();
    void onState4();
    void onState1FullScreen();
    void onState2FullScreen();
    void onState3FullScreen();
    void onState4FullScreen();


public slots:
    void navigateLeft();
    void navigateRight();
    void tickLeft();
    void tickRight();
    void buttonPressed();
    void buttonReleased();
    void buttonLongPressed();
    void wakeup();

};

#endif // GUHTUNEUI_H
