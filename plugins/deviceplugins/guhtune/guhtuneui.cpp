#include "guhtuneui.h"
#include <QSignalTransition>

GuhTuneUi::GuhTuneUi(QWidget *parent):
    QMainWindow(parent)
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, width(), height());

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
    displayCircle->setRect(QRect(QPoint(-640, -640), QPoint(640, 640)));
    displayCircle->setPos(width() / 2, height() / 2);
    displayCircle->setPen(QPen(Qt::white,5));

    double itemScale = 5.05;

    QRectF itemRect = QRect(QPoint(-displayCircle->rect().height() / itemScale, -displayCircle->rect().height() / itemScale), QPoint(displayCircle->rect().height() / itemScale, displayCircle->rect().height() / itemScale));
    QPointF position1(width() / 2 , height() / 2 + 370);
    QPointF position2(width() / 2 + 370, height() / 2 );
    QPointF position3(width() / 2 , height() / 2 - 370);
    QPointF position4(width() / 2 - 370, height() / 2 );

    QRect geometry1(width() / 2 , height() / 2 + 370, itemRect.width(), itemRect.height());
    QRect geometry2(width() / 2 + 370, height() / 2 , itemRect.width(), itemRect.height());
    QRect geometry3(width() / 2 , height() / 2 - 370, itemRect.width(), itemRect.height());
    QRect geometry4(width() / 2 - 370, height() / 2 , itemRect.width(), itemRect.height());

    ItemWidget *one = new ItemWidget(0,Qt::blue);
    one->setGeometry(itemRect);
    one->setPos(position1);

    ItemWidget *two = new ItemWidget(0,Qt::green);
    two->setGeometry(itemRect);
    two->setPos(position2);

    ItemWidget *three = new ItemWidget(0,Qt::red);
    three->setGeometry(itemRect);
    three->setPos(position3);

    ItemWidget *four = new ItemWidget(0,Qt::cyan);
    four->setGeometry(itemRect);
    four->setPos(position4);

    //m_scene->addItem(m_splashItem);
    //m_scene->addItem(m_clock);
    m_scene->addItem(displayCircle);
    m_scene->addItem(one);
    m_scene->addItem(two);
    m_scene->addItem(three);
    m_scene->addItem(four);


    //m_machine = new QStateMachine(this);
    QStateMachine *machine = new QStateMachine(this);
    QState *state1 = new QState(machine);
    QState *state2 = new QState(machine);
    QState *state3 = new QState(machine);
    QState *state4 = new QState(machine);
    machine->setInitialState(state1);

    state1->assignProperty(one,   "geometry", geometry1);
    state1->assignProperty(two,   "geometry", geometry2);
    state1->assignProperty(three, "geometry", geometry3);
    state1->assignProperty(four,  "geometry", geometry4);

    state2->assignProperty(one,   "geometry", geometry4);
    state2->assignProperty(two,   "geometry", geometry1);
    state2->assignProperty(three, "geometry", geometry2);
    state2->assignProperty(four,  "geometry", geometry3);

    state3->assignProperty(one,   "geometry", geometry3);
    state3->assignProperty(two,   "geometry", geometry4);
    state3->assignProperty(three, "geometry", geometry1);
    state3->assignProperty(four,  "geometry", geometry2);

    state4->assignProperty(one,   "geometry", geometry2);
    state4->assignProperty(two,   "geometry", geometry3);
    state4->assignProperty(three, "geometry", geometry4);
    state4->assignProperty(four,  "geometry", geometry1);

    // transition 1->2
    QAbstractTransition *transition12 = state1->addTransition(this, SIGNAL(navigationRight()), state2);
    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup(this);
    animationGroup->addAnimation(new QPropertyAnimation(one,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(two,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(three, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(four,  "geometry"));
    transition12->addAnimation(animationGroup);

    // transition 1->4
    QAbstractTransition *transition14 = state1->addTransition(this, SIGNAL(navigationLeft()), state4);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(one,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(two,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(three, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(four,  "geometry"));
    transition14->addAnimation(animationGroup);

    // transition 2->3
    QAbstractTransition *transition23 = state2->addTransition(this, SIGNAL(navigationRight()), state3);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(one,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(two,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(three, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(four,  "geometry"));
    transition23->addAnimation(animationGroup);

    // transition 2->1
    QAbstractTransition *transition21 = state2->addTransition(this, SIGNAL(navigationLeft()), state1);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(one,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(two,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(three, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(four,  "geometry"));
    transition21->addAnimation(animationGroup);


    // transition 3->4
    QAbstractTransition *transition34 = state3->addTransition(this, SIGNAL(navigationRight()), state4);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(one,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(two,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(three, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(four,  "geometry"));
    transition34->addAnimation(animationGroup);

    // transition 3->2
    QAbstractTransition *transition32 = state3->addTransition(this, SIGNAL(navigationLeft()), state2);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(one,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(two,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(three, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(four,  "geometry"));
    transition32->addAnimation(animationGroup);

    // transition 4->1
    QAbstractTransition *transition41 = state4->addTransition(this, SIGNAL(navigationRight()), state1);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(one,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(two,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(three, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(four,  "geometry"));
    transition41->addAnimation(animationGroup);

    // transition 4->3
    QAbstractTransition *transition43 = state4->addTransition(this, SIGNAL(navigationLeft()), state3);
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(new QPropertyAnimation(one,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(two,   "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(three, "geometry"));
    animationGroup->addAnimation(new QPropertyAnimation(four,  "geometry"));
    transition43->addAnimation(animationGroup);

    machine->start();

    setCentralWidget(m_view);
}

GuhTuneUi::~GuhTuneUi()
{
}

QState *GuhTuneUi::createGeometryState(QObject *w1, const QRect &rect1, QObject *w2, const QRect &rect2, QObject *w3, const QRect &rect3, QObject *w4, const QRect &rect4, QState *parent)
{
    QState *result = new QState(parent);
    result->assignProperty(w1, "geometry", rect1);
    result->assignProperty(w2, "geometry", rect2);
    result->assignProperty(w3, "geometry", rect3);
    result->assignProperty(w4, "geometry", rect4);

    return result;
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
    case Qt::Key_Space:
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



