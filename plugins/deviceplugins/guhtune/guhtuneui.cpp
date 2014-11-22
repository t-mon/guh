#include "guhtuneui.h"
#include <QSignalTransition>

GuhTuneUi::GuhTuneUi(QWidget *parent):
    QMainWindow(parent)
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, width(), height());

    qDebug() << width() << "x" << height();

    m_view = new QGraphicsView(m_scene);
    m_view->setCacheMode(QGraphicsView::CacheBackground);
    m_view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    m_view->setRenderHint(QPainter::Antialiasing);

    m_splashItem = new QGraphicsPixmapItem();
    m_splashItem->setPixmap(QPixmap(":/images/logo.png"));
    m_splashItem->setOffset(-m_splashItem->pixmap().width() / 2, -m_splashItem->pixmap().height() / 2);
    m_splashItem->setPos(width() / 2, height() / 2);

    m_clock = new Clock();
    m_clock->setPos(width() / 2 - m_clock->boundingRect().width() / 2, height() / 2 - m_clock->boundingRect().height() / 2);

    QGraphicsEllipseItem *displayCircle = new QGraphicsEllipseItem();
    displayCircle->setRect(QRect(QPoint(-540, -540), QPoint(540, 540)));
    displayCircle->setPos(width() / 2, height() / 2);
    displayCircle->setPen(QPen(Qt::white,5));

    double itemScale = 5.05;

    QRectF itemRect = QRect(QPoint(-displayCircle->rect().height() / itemScale, -displayCircle->rect().height() / itemScale), QPoint(displayCircle->rect().height() / itemScale, displayCircle->rect().height() / itemScale));
    QPointF position1(width() / 2 , height() / 2 - 320);
    QPointF position2(width() / 2 + 320, height() / 2 );
    QPointF position3(width() / 2 , height() / 2 + 320);
    QPointF position4(width() / 2 - 320, height() / 2 );

    QRect geometry1(position1.rx(), position1.ry(), itemRect.width(), itemRect.height());
    QRect geometry2(position2.rx(), position2.ry(), itemRect.width(), itemRect.height());
    QRect geometry3(position3.rx(), position3.ry(), itemRect.width(), itemRect.height());
    QRect geometry4(position4.rx(), position4.ry(), itemRect.width(), itemRect.height());

    QRect geometryFullScreen(width()/2 , height()/2, displayCircle->rect().width(), displayCircle->rect().height());

    m_itemOne = new ItemWidget(0,Qt::blue,1);
    m_itemOne->setGeometry(itemRect);
    m_itemOne->setPos(position1);
    connect(m_itemOne,SIGNAL(geometryChanged()),m_scene,SLOT(update()));

    m_itemTwo = new ItemWidget(0,Qt::green,2);
    m_itemTwo->setGeometry(itemRect);
    m_itemTwo->setPos(position2);
    connect(m_itemTwo,SIGNAL(geometryChanged()),m_scene,SLOT(update()));

    m_itemThree = new ItemWidget(0,Qt::red,3);
    m_itemThree->setGeometry(itemRect);
    m_itemThree->setPos(position3);
    connect(m_itemThree,SIGNAL(geometryChanged()),m_scene,SLOT(update()));

    m_itemFour = new ItemWidget(0,Qt::cyan,4);
    m_itemFour->setGeometry(itemRect);
    m_itemFour->setPos(position4);
    connect(m_itemFour,SIGNAL(geometryChanged()),m_scene,SLOT(update()));

    //m_scene->addItem(m_splashItem);
    //m_scene->addItem(m_clock);
    m_scene->addItem(displayCircle);
    m_scene->addItem(m_itemOne);
    m_scene->addItem(m_itemTwo);
    m_scene->addItem(m_itemThree);
    m_scene->addItem(m_itemFour);


    //m_machine = new QStateMachine(this);
    QStateMachine *machine = new QStateMachine(this);
    QState *state1 = new QState(machine);
    QState *state2 = new QState(machine);
    QState *state3 = new QState(machine);
    QState *state4 = new QState(machine);
    QState *state1FullScreen = new QState(machine);
    QState *state2FullScreen = new QState(machine);
    QState *state3FullScreen = new QState(machine);
    QState *state4FullScreen = new QState(machine);

    connect(state1,SIGNAL(entered()),this,SLOT(onState1()));
    connect(state2,SIGNAL(entered()),this,SLOT(onState2()));
    connect(state3,SIGNAL(entered()),this,SLOT(onState3()));
    connect(state4,SIGNAL(entered()),this,SLOT(onState4()));

    state1->assignProperty(m_itemOne,   "geometry", geometry1);
    state1->assignProperty(m_itemTwo,   "geometry", geometry2);
    state1->assignProperty(m_itemThree, "geometry", geometry3);
    state1->assignProperty(m_itemFour,  "geometry", geometry4);

    state2->assignProperty(m_itemOne,   "geometry", geometry4);
    state2->assignProperty(m_itemTwo,   "geometry", geometry1);
    state2->assignProperty(m_itemThree, "geometry", geometry2);
    state2->assignProperty(m_itemFour,  "geometry", geometry3);

    state3->assignProperty(m_itemOne,   "geometry", geometry3);
    state3->assignProperty(m_itemTwo,   "geometry", geometry4);
    state3->assignProperty(m_itemThree, "geometry", geometry1);
    state3->assignProperty(m_itemFour,  "geometry", geometry2);

    state4->assignProperty(m_itemOne,   "geometry", geometry2);
    state4->assignProperty(m_itemTwo,   "geometry", geometry3);
    state4->assignProperty(m_itemThree, "geometry", geometry4);
    state4->assignProperty(m_itemFour,  "geometry", geometry1);

    state1FullScreen->assignProperty(m_itemOne,   "geometry", geometryFullScreen);
    state1FullScreen->assignProperty(m_itemTwo,   "geometry", geometry2);
    state1FullScreen->assignProperty(m_itemThree, "geometry", geometry3);
    state1FullScreen->assignProperty(m_itemFour,  "geometry", geometry4);

    state2FullScreen->assignProperty(m_itemOne,   "geometry", geometry4);
    state2FullScreen->assignProperty(m_itemTwo,   "geometry", geometryFullScreen);
    state2FullScreen->assignProperty(m_itemThree, "geometry", geometry2);
    state2FullScreen->assignProperty(m_itemFour,  "geometry", geometry3);

    state3FullScreen->assignProperty(m_itemOne,   "geometry", geometry3);
    state3FullScreen->assignProperty(m_itemTwo,   "geometry", geometry4);
    state3FullScreen->assignProperty(m_itemThree, "geometry", geometryFullScreen);
    state3FullScreen->assignProperty(m_itemFour,  "geometry", geometry2);

    state4FullScreen->assignProperty(m_itemOne,   "geometry", geometry2);
    state4FullScreen->assignProperty(m_itemTwo,   "geometry", geometry3);
    state4FullScreen->assignProperty(m_itemThree, "geometry", geometry4);
    state4FullScreen->assignProperty(m_itemFour,  "geometry", geometryFullScreen);

    // ONE ##############################################################################
    // transition 1->2
    QAbstractTransition *transition12 = state1->addTransition(this, SIGNAL(navigationRight()), state2);
    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup(this);
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition12->addAnimation(animationGroup);

    // transition 1->4
    QAbstractTransition *transition14 = state1->addTransition(this, SIGNAL(navigationLeft()), state4);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition14->addAnimation(animationGroup);

    // transition 1->Overview
    QAbstractTransition *transition1O = state1->addTransition(this, SIGNAL(enterOverview()), state1FullScreen);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition1O->addAnimation(animationGroup);

    // transition Overview -> 1
    QAbstractTransition *transitionO1 = state1FullScreen->addTransition(this, SIGNAL(exitOverview()), state1);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transitionO1->addAnimation(animationGroup);


    // TWO ##############################################################################
    // transition 2->3
    QAbstractTransition *transition23 = state2->addTransition(this, SIGNAL(navigationRight()), state3);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition23->addAnimation(animationGroup);

    // transition 2->1
    QAbstractTransition *transition21 = state2->addTransition(this, SIGNAL(navigationLeft()), state1);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition21->addAnimation(animationGroup);

    // transition 2->Overview
    QAbstractTransition *transition2O = state2->addTransition(this, SIGNAL(enterOverview()), state2FullScreen);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition2O->addAnimation(animationGroup);

    // transition Overview -> 2
    QAbstractTransition *transitionO2 = state2FullScreen->addTransition(this, SIGNAL(exitOverview()), state2);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transitionO2->addAnimation(animationGroup);



    // THREE ##############################################################################
    // transition 3->4
    QAbstractTransition *transition34 = state3->addTransition(this, SIGNAL(navigationRight()), state4);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition34->addAnimation(animationGroup);

    // transition 3->2
    QAbstractTransition *transition32 = state3->addTransition(this, SIGNAL(navigationLeft()), state2);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition32->addAnimation(animationGroup);

    // transition 3->Overview
    QAbstractTransition *transition3O = state3->addTransition(this, SIGNAL(enterOverview()), state3FullScreen);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition3O->addAnimation(animationGroup);

    // transition Overview -> 3
    QAbstractTransition *transitionO3 = state3FullScreen->addTransition(this, SIGNAL(exitOverview()), state3);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transitionO3->addAnimation(animationGroup);


    // FOUR ##############################################################################
    // transition 4->1
    QAbstractTransition *transition41 = state4->addTransition(this, SIGNAL(navigationRight()), state1);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition41->addAnimation(animationGroup);

    // transition 4->3
    QAbstractTransition *transition43 = state4->addTransition(this, SIGNAL(navigationLeft()), state3);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition43->addAnimation(animationGroup);

    // transition 4->Overview
    QAbstractTransition *transition4O = state4->addTransition(this, SIGNAL(enterOverview()), state4FullScreen);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transition4O->addAnimation(animationGroup);

    // transition Overview -> 4
    QAbstractTransition *transitionO4 = state4FullScreen->addTransition(this, SIGNAL(exitOverview()), state4);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(m_itemOne,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemTwo,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemThree, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(m_itemFour,  "geometry"));
    transitionO4->addAnimation(animationGroup);


    machine->setInitialState(state1);
    machine->start();
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
        emit navigationLeft();
        break;
    case Qt::Key_D:
        qDebug() << "navigate right";
        emit navigationRight();
        break;
    case Qt::Key_W:
        qDebug() << "overview";
        emit enterOverview();
        break;
    case Qt::Key_S:
        qDebug() << "exit overview";
        emit exitOverview();
        break;
    case Qt::Key_Space:
        //m_view->update();
        m_scene->update();
        break;
    case Qt::Key_T:{
        bool visible = m_clock->isVisible();
        m_clock->setVisible(!visible);
        break;
    }
    default:
        qDebug() << keyEvent->text() << "key pressed";
    }
}

void GuhTuneUi::onState1()
{
    m_itemOne->setZValue(0);
    m_itemTwo->setZValue(-1);
    m_itemThree->setZValue(-2);
    m_itemFour->setZValue(-3);

}

void GuhTuneUi::onState2()
{
    m_itemOne->setZValue(-3);
    m_itemTwo->setZValue(0);
    m_itemThree->setZValue(-1);
    m_itemFour->setZValue(-2);
}

void GuhTuneUi::onState3()
{
    m_itemOne->setZValue(-2);
    m_itemTwo->setZValue(-3);
    m_itemThree->setZValue(0);
    m_itemFour->setZValue(-1);
}

void GuhTuneUi::onState4()
{
    m_itemOne->setZValue(-1);
    m_itemTwo->setZValue(-2);
    m_itemThree->setZValue(-3);
    m_itemFour->setZValue(0);
}



