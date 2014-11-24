#include "guhtuneuiserver.h"

GuhTuneUiServer::GuhTuneUiServer(QObject *parent):
    QObject(parent)
{
}

void GuhTuneUiServer::sendData(const QByteArray &data)
{
    foreach (QTcpSocket *client, m_clientList) {
        client->write(data);
    }
}

void GuhTuneUiServer::newClientConnected()
{
    // got a new client connected
    QTcpServer *server = qobject_cast<QTcpServer*>(sender());
    QTcpSocket *newConnection = server->nextPendingConnection();
    qDebug() << "new client connected:" << newConnection->peerAddress().toString();

    // append the new client to the client list
    m_clientList.append(newConnection);

    connect(newConnection, SIGNAL(readyRead()),this, SLOT(readPackage()));
    connect(newConnection,SIGNAL(disconnected()),this, SLOT(slotClientDisconnected()));
}

void GuhTuneUiServer::readPackage()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "-----------> data comming from" << client->peerAddress().toString();
    QByteArray message;
    while(client->canReadLine()){
        QByteArray dataLine = client->readLine();
        qDebug() << "line in:" << dataLine;
        message.append(dataLine);
        if(dataLine.endsWith('\n')){
            emit dataAvailable(message);
            message.clear();
        }
    }
}

void GuhTuneUiServer::slotClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "----> guhTune UI server: client disconnected:" << client->peerAddress().toString();
    m_clientList.removeOne(client);
    delete client;
}

bool GuhTuneUiServer::startServer()
{
    // Listen on all Networkinterfaces
    foreach(const QHostAddress &address, QNetworkInterface::allAddresses()){
        QTcpServer *server = new QTcpServer(this);
        if(server->listen(address, 9876)) {
            qDebug() << " ----> guhTune UI server started and listening on port" << server->serverPort() << "and ip " << address.toString();
            connect(server, SIGNAL(newConnection()), SLOT(newClientConnected()));

            m_serverList.append(server);
        } else {
            qDebug() << "ERROR: can not listening to" << address.toString();
            delete server;
            return false;
        }
    }
    return true;
}

bool GuhTuneUiServer::stopServer()
{
    return true;
}

void GuhTuneUiServer::navigateLeft()
{
}

void GuhTuneUiServer::navigateRight()
{
}

void GuhTuneUiServer::tickLeft()
{
}

void GuhTuneUiServer::tickRight()
{
}

void GuhTuneUiServer::buttonPressed()
{
}

void GuhTuneUiServer::buttonReleased()
{
}

void GuhTuneUiServer::buttonLongPressed()
{
}

void GuhTuneUiServer::wakeup()
{
}

void GuhTuneUiServer::sleep()
{
}



