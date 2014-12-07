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

#ifndef GUHTUNEUISERVER_H
#define GUHTUNEUISERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QJsonDocument>

class GuhTuneUiServer : public QObject
{
    Q_OBJECT

public:
    explicit GuhTuneUiServer(QObject *parent = 0);
    void sendData(const QByteArray &data);

private:
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
    void handDetected();
    void handDisappeard();

};

#endif // GUHTUNEUISERVER_H
