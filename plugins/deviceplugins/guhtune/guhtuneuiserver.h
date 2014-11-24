#ifndef GUHTUNEUISERVER_H
#define GUHTUNEUISERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostAddress>

class GuhTuneUiServer : public QObject
{
    Q_OBJECT

public:
    explicit GuhTuneUiServer(QObject *parent = 0);

private:
    void sendData(const QByteArray &data);
    QList<QTcpServer*> m_serverList;
    QList<QTcpSocket*> m_clientList;

private slots:
    void newClientConnected();
    void readPackage();
    void slotClientDisconnected();

signals:
    void clientConnected(const QUuid &clientId);
    void clientDisconnected(const QUuid &clientId);
    void dataAvailable(const QByteArray &data);

    void itemPressed(int itemNumber);
    void itemValueChanged(int itemNumber, int value);

public slots:
    bool startServer();
    bool stopServer();

    void navigateLeft();
    void navigateRight();
    void tickLeft();
    void tickRight();
    void buttonPressed();
    void buttonReleased();
    void buttonLongPressed();
    void wakeup();
    void sleep();

};

#endif // GUHTUNEUISERVER_H
