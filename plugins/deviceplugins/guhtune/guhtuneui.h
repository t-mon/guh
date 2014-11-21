#ifndef GUHTUNEUI_H
#define GUHTUNEUI_H

#include <QObject>

#include <QtWidgets>
#include <QDebug>

class GuhTuneUi : public QMainWindow
{
    Q_OBJECT

public:
    enum Rotation {
        RotationLeft,
        RotationRight
    };

    explicit GuhTuneUi(QObject *parent = 0);
    ~GuhTuneUi();

signals:
    // Called from ui
    void toggle(int index);
    void setValue(int index, int value);

    // Called from plugin code
    void buttonPressed();
    void buttonReleased();
    void smallStep(Rotation rotation);
    void bigStep(Rotation rotation);
    void wakeup();

private:
    QGraphicsView *m_view;
    void paintEvent(QPaintEvent *e);
    void drawSpashScreen();
};

#endif // GUHTUNEUI_H
