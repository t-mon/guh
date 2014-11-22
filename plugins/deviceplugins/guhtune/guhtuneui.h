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
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;

    QGraphicsPixmapItem *m_splashItem;
    Clock *m_clock;
    ItemWidget *m_itemOne;
    ItemWidget *m_itemTwo;
    ItemWidget *m_itemThree;
    ItemWidget *m_itemFour;

protected:
    void keyPressEvent(QKeyEvent *keyEvent);

signals:
    // Called from ui
    void toggle(int index);
    void setValue(int index, int value);

    void navigationLeft();
    void navigationRight();

    void enterOverview();
    void exitOverview();

    // Called from plugin code
    void buttonPressed();
    void buttonReleased();
    void wakeup();

private slots:
    void onState1();
    void onState2();
    void onState3();
    void onState4();

public slots:
    void navigateLeft();
    void navigateRight();



};

#endif // GUHTUNEUI_H
