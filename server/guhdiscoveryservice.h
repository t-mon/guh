#ifndef GUHDISCOVERYSERVICE_H
#define GUHDISCOVERYSERVICE_H

#include <QObject>
#include <QUdpSocket>

class GuhDiscoveryService : public QObject
{
    Q_OBJECT
public:
    explicit GuhDiscoveryService(QObject *parent = nullptr);

private:
    QUdpSocket *m_udpSocket;


private slots:
    void sendDiscoveryResponse(const QHostAddress &address, const quint16 &port);
    void readData();

public slots:
    void enable();
    void disable();

};

#endif // GUHDISCOVERYSERVICE_H
