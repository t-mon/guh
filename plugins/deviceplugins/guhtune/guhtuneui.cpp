#include "guhtuneui.h"

GuhTuneUi::GuhTuneUi(QObject *parent):
    QWidget(parent)
{
    QGraphicsScene scene;

    m_view = new QGraphicsView(&scene);
    m_view->setCacheMode(QGraphicsView::CacheBackground);
    m_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    m_view->showFullScreen();
}

GuhTuneUi::~GuhTuneUi()
{
}

void GuhTuneUi::drawSpashScreen()
{


}

