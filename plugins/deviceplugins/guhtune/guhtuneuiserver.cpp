/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  This file is part of guh.                                              *
 *                                                                         *
 *  Guh is free software: you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, version 2 of the License.                *
 *                                                                         *
 *  Guh is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with guh. If not, see <http://www.gnu.org/licenses/>.            *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "guhtuneuiserver.h"

GuhTuneUiServer::GuhTuneUiServer(QObject *parent):
    QObject(parent)
{
}

void GuhTuneUiServer::sendData(const QByteArray &data)
{
    foreach (QTcpSocket *client, m_clientList) {
        client->write(data + "\n");
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
            qDebug() << "              server started and listening on port" << server->serverPort() << "and ip " << address.toString();
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

void GuhTuneUiServer::navigateLeft()
{
    sendData("NL");
}

void GuhTuneUiServer::navigateRight()
{
    sendData("NR");
}

void GuhTuneUiServer::tickLeft()
{
    sendData("L");
}

void GuhTuneUiServer::tickRight()
{
    sendData("R");
}

void GuhTuneUiServer::buttonPressed()
{
    sendData("BP");
}

void GuhTuneUiServer::buttonReleased()
{
    sendData("BR");
}

void GuhTuneUiServer::buttonLongPressed()
{
    sendData("BLP");
}

void GuhTuneUiServer::handDetected()
{
    sendData("HDE");
}

void GuhTuneUiServer::handDisappeard()
{
    sendData("HDI");
}


