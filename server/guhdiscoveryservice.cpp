#include "guhdiscoveryservice.h"
#include "loggingcategories.h"
#include "guhcore.h"

GuhDiscoveryService::GuhDiscoveryService(QObject *parent) :
    QObject(parent),
    m_udpSocket(nullptr)
{

}

void GuhDiscoveryService::sendDiscoveryResponse(const QHostAddress &address, const quint16 &port)
{
    // TODO: create defined response with information
    QVariantMap dataMap;
    dataMap.insert("name", GuhCore::instance()->configuration()->serverName());
    dataMap.insert("server", "guhIO");
    dataMap.insert("version", GUH_VERSION_STRING);
    dataMap.insert("uuid", GuhCore::instance()->configuration()->serverUuid().toString());

    QByteArray data("Hello!");
    m_udpSocket->writeDatagram(data, address, port);
}

void GuhDiscoveryService::readData()
{
    QUdpSocket *socket = qobject_cast<QUdpSocket*>(sender());
    QByteArray datagram; QHostAddress senderAddress; quint16 senderPort;

    while (socket->hasPendingDatagrams()) {
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);
    }

    qCDebug(dcDiscoveryService()) << "Recived data from" << senderAddress.toString() << datagram;
    if (datagram == "Hello guh!")
        sendDiscoveryResponse(senderAddress, senderPort);

}

void GuhDiscoveryService::enable()
{
    qCDebug(dcDiscoveryService) << "Start guh discovery service";
    m_udpSocket = new QUdpSocket(this);

    m_udpSocket->bind(QHostAddress::Any, 2324, QAbstractSocket::ShareAddress);

    connect(m_udpSocket, &QUdpSocket::readyRead, this, &GuhDiscoveryService::readData);
}

void GuhDiscoveryService::disable()
{
    qCDebug(dcDiscoveryService) << "Shut down guh discovery service";

    m_udpSocket->deleteLater();
    m_udpSocket = nullptr;
}
