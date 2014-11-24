#include "guhtuneui.h"
#include <QSignalTransition>

GuhTuneUi::GuhTuneUi(QWidget *parent):
    QMainWindow(parent)
{
    int displaySize = 300   ;
    resize(displaySize +20 ,displaySize +20);
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, displaySize, displaySize);

    qDebug() << "UI size =" << displaySize << "x" << displaySize;

    m_view = new QGraphicsView(m_scene);
    m_view->setCacheMode(QGraphicsView::CacheBackground);
    m_view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    m_view->setRenderHint(QPainter::Antialiasing);

    m_splashItem = new QGraphicsPixmapItem();
    m_splashItem->setPixmap(QPixmap(":/images/logo.png"));
    m_splashItem->setOffset(-m_splashItem->pixmap().width() / 2, -m_splashItem->pixmap().height() / 2);
    m_splashItem->setPos(displaySize / 2, displaySize / 2);

//    QGraphicsEllipseItem *displayCircle = new QGraphicsEllipseItem();
//    displayCircle->setRect(QRect(QPoint(-displaySize/2, -displaySize/2), QPoint(displaySize/2, displaySize/2)));
//    displayCircle->setPos(displaySize / 2, displaySize / 2);
//    displayCircle->setPen(QPen(Qt::white,5));

    double itemScale = 5.05;

    QRectF itemRect = QRect(QPoint(-displaySize / itemScale, -displaySize / itemScale), QPoint(displaySize / itemScale, displaySize / itemScale));

    QPointF position1(displaySize / 2 , displaySize / 2 - displaySize / 2 + itemRect.height() / 2);
    QPointF position2(displaySize / 2 + displaySize / 2 - itemRect.width() / 2, displaySize / 2 );
    QPointF position3(displaySize / 2 , displaySize / 2 + displaySize / 2 - itemRect.height() / 2);
    QPointF position4(displaySize / 2 - displaySize / 2 + itemRect.width() / 2, displaySize / 2 );

    QRect geometry1(position1.rx(), position1.ry(), itemRect.width(), itemRect.height());
    QRect geometry2(position2.rx(), position2.ry(), itemRect.width(), itemRect.height());
    QRect geometry3(position3.rx(), position3.ry(), itemRect.width(), itemRect.height());
    QRect geometry4(position4.rx(), position4.ry(), itemRect.width(), itemRect.height());

    QRect geometryFullScreen(displaySize/2 , displaySize/2, displaySize, displaySize);

    // CLOCK ##################################################################
    m_clock = new Clock();
    //m_clock->setPos(displaySize / 2, displaySize / 2);
    m_clock->setZValue(1);
    m_clock->setGeometry(QRect(QPoint(0,0), QPoint(displaySize, displaySize)));
    m_clock->setVisible(false);
    connect(m_clock,SIGNAL(clockChanged()),m_view,SLOT(update()));

    m_sleepTimer = new QTimer(this);
    m_sleepTimer->setInterval(5000);
    m_sleepTimer->setSingleShot(true);
    connect(m_sleepTimer, SIGNAL(timeout()),this, SLOT(sleep()));

    // ITEMS ##################################################################
    m_itemOne = new ItemOne();
    m_itemOne->setGeometry(itemRect);
    m_itemOne->setPos(position1);
    connect(m_itemOne,SIGNAL(geometryChanged()),m_scene,SLOT(update()));

    m_itemTwo = new ItemTwo();
    m_itemTwo->setGeometry(itemRect);
    m_itemTwo->setPos(position2);
    connect(m_itemTwo,SIGNAL(geometryChanged()),m_scene,SLOT(update()));

    m_itemThree = new ItemThree();
    m_itemThree->setGeometry(itemRect);
    m_itemThree->setPos(position3);
    connect(m_itemThree,SIGNAL(geometryChanged()),m_scene,SLOT(update()));

    m_itemFour = new ItemFour();
    m_itemFour->setGeometry(itemRect);
    m_itemFour->setPos(position4);
    connect(m_itemFour,SIGNAL(geometryChanged()),m_scene,SLOT(update()));

    m_itemOne->setZValue(0);
    m_itemTwo->setZValue(-1);
    m_itemThree->setZValue(-2);
    m_itemFour->setZValue(-3);

    m_scene->addItem(m_clock);
    //m_scene->addItem(displayCircle);
    m_scene->addItem(m_itemOne);
    m_scene->addItem(m_itemTwo);
    m_scene->addItem(m_itemThree);
    m_scene->addItem(m_itemFour);

    // STATE MACHINE ##################################################################
    m_machine = new QStateMachine(this);
    m_state1 = new QState(m_machine);
    m_state2 = new QState(m_machine);
    m_state3 = new QState(m_machine);
    m_state4 = new QState(m_machine);
    m_state1FullScreen = new QState(m_machine);
    m_state2FullScreen = new QState(m_machine);
    m_state3FullScreen = new QState(m_machine);
    m_state4FullScreen = new QState(m_machine);

    connect(m_state1,SIGNAL(entered()),this,SLOT(onState1()));
    connect(m_state2,SIGNAL(entered()),this,SLOT(onState2()));
    connect(m_state3,SIGNAL(entered()),this,SLOT(onState3()));
    connect(m_state4,SIGNAL(entered()),this,SLOT(onState4()));
    connect(m_state1FullScreen,SIGNAL(entered()),this,SLOT(onState1FullScreen()));
    connect(m_state2FullScreen,SIGNAL(entered()),this,SLOT(onState2FullScreen()));
    connect(m_state3FullScreen,SIGNAL(entered()),this,SLOT(onState3FullScreen()));
    connect(m_state4FullScreen,SIGNAL(entered()),this,SLOT(onState4FullScreen()));

    m_state1->assignProperty(m_itemOne,   "geometry", geometry1);
    m_state1->assignProperty(m_itemTwo,   "geometry", geometry2);
    m_state1->assignProperty(m_itemThree, "geometry", geometry3);
    m_state1->assignProperty(m_itemFour,  "geometry", geometry4);

    m_state2->assignProperty(m_itemOne,   "geometry", geometry4);
    m_state2->assignProperty(m_itemTwo,   "geometry", geometry1);
    m_state2->assignProperty(m_itemThree, "geometry", geometry2);
    m_state2->assignProperty(m_itemFour,  "geometry", geometry3);

    m_state3->assignProperty(m_itemOne,   "geometry", geometry3);
    m_state3->assignProperty(m_itemTwo,   "geometry", geometry4);
    m_state3->assignProperty(m_itemThree, "geometry", geometry1);
    m_state3->assignProperty(m_itemFour,  "geometry", geometry2);

    m_state4->assignProperty(m_itemOne,   "geometry", geometry2);
    m_state4->assignProperty(m_itemTwo,   "geometry", geometry3);
    m_state4->assignProperty(m_itemThree, "geometry", geometry4);
    m_state4->assignProperty(m_itemFour,  "geometry", geometry1);

    m_state1FullScreen->assignProperty(m_itemOne,   "geometry", geometryFullScreen);
    m_state1FullScreen->assignProperty(m_itemTwo,   "geometry", geometry2);
    m_state1FullScreen->assignProperty(m_itemThree, "geometry", geometry3);
    m_state1FullScreen->assignProperty(m_itemFour,  "geometry", geometry4);

    m_state2FullScreen->assignProperty(m_itemOne,   "geometry", geometry4);
    m_state2FullScreen->assignProperty(m_itemTwo,   "geometry", geometryFullScreen);
    m_state2FullScreen->assignProperty(m_itemThree, "geometry", geometry2);
    m_state2FullScreen->assignProperty(m_itemFour,  "geometry", geometry3);

    m_state3FullScreen->assignProperty(m_itemOne,   "geometry", geometry3);
    m_state3FullScreen->assignProperty(m_itemTwo,   "geometry", geometry4);
    m_state3FullScreen->assignProperty(m_itemThree, "geometry", geometryFullScreen);
    m_state3FullScreen->assignProperty(m_itemFour,  "geometry", geometry2);

    m_state4FullScreen->assignProperty(m_itemOne,   "geometry", geometry2);
    m_state4FullScreen->assignProperty(m_itemTwo,   "geometry", geometry3);
    m_state4FullScreen->assignProperty(m_itemThree, "geometry", geometry4);
    m_state4FullScreen->assignProperty(m_itemFour,  "geometry", geometryFullScreen);

    // TRANSITIONS ######################################################################
    // ONE ##############################################################################
    // transition 1->2
    QAbstractTransition *transition12 = m_state1->addTransition(this, SIGNAL(navigationRight()), m_state2);
    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup(this);
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition12->addAnimation(animationGroup);

    // transition 1->4
    QAbstractTransition *transition14 = m_state1->addTransition(this, SIGNAL(navigationLeft()), m_state4);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition14->addAnimation(animationGroup);

    // transition 1->Overview
    QAbstractTransition *transition1O = m_state1->addTransition(this, SIGNAL(enterOverview()), m_state1FullScreen);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition1O->addAnimation(animationGroup);

    // transition Overview -> 1
    QAbstractTransition *transitionO1 = m_state1FullScreen->addTransition(this, SIGNAL(exitOverview()), m_state1);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transitionO1->addAnimation(animationGroup);


    // TWO ##############################################################################
    // transition 2->3
    QAbstractTransition *transition23 = m_state2->addTransition(this, SIGNAL(navigationRight()), m_state3);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition23->addAnimation(animationGroup);

    // transition 2->1
    QAbstractTransition *transition21 = m_state2->addTransition(this, SIGNAL(navigationLeft()), m_state1);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition21->addAnimation(animationGroup);

    // transition 2->Overview
    QAbstractTransition *transition2O = m_state2->addTransition(this, SIGNAL(enterOverview()), m_state2FullScreen);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition2O->addAnimation(animationGroup);

    // transition Overview -> 2
    QAbstractTransition *transitionO2 = m_state2FullScreen->addTransition(this, SIGNAL(exitOverview()), m_state2);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transitionO2->addAnimation(animationGroup);



    // THREE ##############################################################################
    // transition 3->4
    QAbstractTransition *transition34 = m_state3->addTransition(this, SIGNAL(navigationRight()), m_state4);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition34->addAnimation(animationGroup);

    // transition 3->2
    QAbstractTransition *transition32 = m_state3->addTransition(this, SIGNAL(navigationLeft()), m_state2);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition32->addAnimation(animationGroup);

    // transition 3->Overview
    QAbstractTransition *transition3O = m_state3->addTransition(this, SIGNAL(enterOverview()), m_state3FullScreen);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition3O->addAnimation(animationGroup);

    // transition Overview -> 3
    QAbstractTransition *transitionO3 = m_state3FullScreen->addTransition(this, SIGNAL(exitOverview()), m_state3);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transitionO3->addAnimation(animationGroup);


    // FOUR ##############################################################################
    // transition 4->1
    QAbstractTransition *transition41 = m_state4->addTransition(this, SIGNAL(navigationRight()), m_state1);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition41->addAnimation(animationGroup);

    // transition 4->3
    QAbstractTransition *transition43 = m_state4->addTransition(this, SIGNAL(navigationLeft()), m_state3);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition43->addAnimation(animationGroup);

    // transition 4->Overview
    QAbstractTransition *transition4O = m_state4->addTransition(this, SIGNAL(enterOverview()), m_state4FullScreen);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition4O->addAnimation(animationGroup);

    // transition Overview -> 4
    QAbstractTransition *transitionO4 = m_state4FullScreen->addTransition(this, SIGNAL(exitOverview()), m_state4);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transitionO4->addAnimation(animationGroup);

    m_currentState = m_state1FullScreen;
    m_machine->setInitialState(m_state1FullScreen);
    m_machine->start();
    m_sleepTimer->start();
    setCentralWidget(m_view);
}

GuhTuneUi::~GuhTuneUi()
{
}

void GuhTuneUi::keyPressEvent(QKeyEvent *keyEvent)
{
    switch (keyEvent->key()) {
    case Qt::Key_A:
        qDebug() << "navigate left";
        wakeup();
        emit navigationLeft();
        break;
    case Qt::Key_D:
        qDebug() << "navigate right";
        wakeup();
        emit navigationRight();
        break;
    case Qt::Key_W:
        qDebug() << "overview";
        wakeup();
        emit enterOverview();
        break;
    case Qt::Key_S:
        qDebug() << "exit overview";
        wakeup();
        emit exitOverview();
        break;
    case Qt::Key_2:
        wakeup();
        tickRight();
        break;
    case Qt::Key_1:
        wakeup();
        tickLeft();
        break;
    case Qt::Key_Q:
        wakeup();
        break;
    }
    keyEvent->ignore();
}

void GuhTuneUi::onState1()
{
    m_itemOne->setZValue(0);
    m_itemTwo->setZValue(-1);
    m_itemThree->setZValue(-2);
    m_itemFour->setZValue(-3);

    m_currentState = m_state1;
}

void GuhTuneUi::onState2()
{
    m_itemOne->setZValue(-3);
    m_itemTwo->setZValue(0);
    m_itemThree->setZValue(-1);
    m_itemFour->setZValue(-2);

    m_currentState = m_state2;
}

void GuhTuneUi::onState3()
{
    m_itemOne->setZValue(-2);
    m_itemTwo->setZValue(-3);
    m_itemThree->setZValue(0);
    m_itemFour->setZValue(-1);

    m_currentState = m_state3;
}

void GuhTuneUi::onState4()
{
    m_itemOne->setZValue(-1);
    m_itemTwo->setZValue(-2);
    m_itemThree->setZValue(-3);
    m_itemFour->setZValue(0);

    m_currentState = m_state4;
}

void GuhTuneUi::onState1FullScreen()
{
    m_currentState = m_state1FullScreen;
}

void GuhTuneUi::onState2FullScreen()
{
    m_currentState = m_state2FullScreen;
}

void GuhTuneUi::onState3FullScreen()
{
    m_currentState = m_state3FullScreen;
}

void GuhTuneUi::onState4FullScreen()
{
    m_currentState = m_state4FullScreen;
}

void GuhTuneUi::navigateLeft()
{
    wakeup();
    if (m_currentState == m_state1
            || m_currentState == m_state2
            || m_currentState == m_state3
            || m_currentState == m_state4) {
        emit navigateLeft();
        m_scene->update();
    }
}

void GuhTuneUi::navigateRight()
{
    wakeup();
    if (m_currentState == m_state1
            || m_currentState == m_state2
            || m_currentState == m_state3
            || m_currentState == m_state4) {
        emit navigateRight();
        m_scene->update();
    }
}

void GuhTuneUi::tickLeft()
{
    wakeup();
    if (m_currentState == m_state1FullScreen) {
        m_itemOne->tickLeft();
    } else if (m_currentState == m_state2FullScreen) {
        m_itemTwo->tickLeft();
    } else if (m_currentState == m_state4FullScreen) {
        m_itemFour->tickLeft();
    }
    m_scene->update();
}

void GuhTuneUi::tickRight()
{
    wakeup();
    if (m_currentState == m_state1FullScreen) {
        m_itemOne->tickRight();
    } else if (m_currentState == m_state2FullScreen) {
        m_itemTwo->tickRight();
    } else if (m_currentState == m_state4FullScreen) {
        m_itemFour->tickRight();
    }
    m_scene->update();
}

void GuhTuneUi::buttonPressed()
{
    wakeup();
}

void GuhTuneUi::buttonReleased()
{
    wakeup();
    if (m_currentState == m_state1
            || m_currentState == m_state2
            || m_currentState == m_state3
            || m_currentState == m_state4) {
        emit enterOverview();
    }
}

void GuhTuneUi::buttonLongPressed()
{
    wakeup();
    if (m_currentState == m_state1FullScreen
            || m_currentState == m_state2FullScreen
            || m_currentState == m_state3FullScreen
            || m_currentState == m_state4FullScreen) {
        emit exitOverview();
    }
}

void GuhTuneUi::wakeup()
{
    m_sleepTimer->start();
    m_clock->setVisible(false);
    m_view->update();
}

void GuhTuneUi::sleep()
{
    m_sleepTimer->start();
    m_clock->setVisible(true);
    m_view->update();
}



