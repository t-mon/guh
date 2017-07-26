#include "guhdiscoveryservice.h"
#include "loggingcategories.h"
#include "guhcore.h"

#include <QJsonDocument>

GuhDiscoveryService::GuhDiscoveryService(QObject *parent) :
    QObject(parent),
    m_udpSocket(nullptr),
    m_port(2324)
{

}

void GuhDiscoveryService::sendDiscoveryResponse(const QHostAddress &address)
{
    // TODO: create defined response with information
    qCDebug(dcDiscoveryService()) << "Sending response to client 10 times" << address.toString();

    QVariantMap dataMap;
    dataMap.insert("name", GuhCore::instance()->configuration()->serverName());
    dataMap.insert("server", "guhIO");
    dataMap.insert("version", GUH_VERSION_STRING);
    dataMap.insert("protocol version", JSON_PROTOCOL_VERSION);
    dataMap.insert("language", GuhCore::instance()->configuration()->locale().name());
    dataMap.insert("uuid", GuhCore::instance()->configuration()->serverUuid().toString());
    dataMap.insert("tcpServerPort", GuhCore::instance()->configuration()->tcpServerPort());
    dataMap.insert("webServerPort", GuhCore::instance()->configuration()->webServerPort());
    dataMap.insert("webSocketServerPort", GuhCore::instance()->configuration()->webSocketPort());

    m_udpSocket->writeDatagram(QJsonDocument::fromVariant(dataMap).toJson(QJsonDocument::Compact) + "\n", address, m_port);
}

void GuhDiscoveryService::readData()
{
    QUdpSocket *socket = qobject_cast<QUdpSocket*>(sender());
    QByteArray datagram; QHostAddress senderAddress; quint16 senderPort;

    while (socket->hasPendingDatagrams()) {
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);
    }

    qCDebug(dcDiscoveryService()) << "Received data from" << senderAddress.toString() << datagram;
    if (datagram.contains("Hello guh!"))
        sendDiscoveryResponse(senderAddress);

}

void GuhDiscoveryService::enable()
{
    if (m_udpSocket) {
        m_udpSocket->deleteLater();
        m_udpSocket = nullptr;
    }

    m_udpSocket = new QUdpSocket(this);

    if (!m_udpSocket->bind(QHostAddress::Any, m_port, QAbstractSocket::ShareAddress)) {
        qCWarning(dcDiscoveryService()) << "Could not bind to port" << m_port;
        m_udpSocket->deleteLater();
        m_udpSocket = nullptr;
        return;
    }

    qCDebug(dcDiscoveryService) << "Started guh discovery service successfully on port" << m_port;
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &GuhDiscoveryService::readData);
}

void GuhDiscoveryService::disable()
{
    qCDebug(dcDiscoveryService) << "Shut down guh discovery service";

    m_udpSocket->deleteLater();
    m_udpSocket = nullptr;
}
