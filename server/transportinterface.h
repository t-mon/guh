/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2015 Simon Stuerz <simon.stuerz@guh.guru>                *
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

#ifndef TRANSPORTINTERFACE_H
#define TRANSPORTINTERFACE_H

#include <QVariant>
#include <QString>
#include <QList>
#include <QUuid>

namespace guhserver {

class TransportInterface : public QObject
{
    Q_OBJECT
public:
    explicit TransportInterface(const bool &authenticationEnabled, QObject *parent = 0);
    virtual ~TransportInterface() = 0;

    virtual void sendData(const QUuid &clientId, const QVariantMap &data) = 0;
    virtual void sendData(const QList<QUuid> &clients, const QVariantMap &data) = 0;

    void sendResponse(const QUuid &clientId, int commandId, const QVariantMap &params = QVariantMap());
    void sendErrorResponse(const QUuid &clientId, int commandId, const QString &error);

protected:
    bool m_authenticationEnabled;
    void validateMessage(const QUuid &clientId, const QByteArray &data);

signals:
    void clientConnected(const QUuid &clientId);
    void clientDisconnected(const QUuid &clientId);
    void dataAvailable(const QUuid &clientId, const QString &targetNamespace, const QString &method, const QVariantMap &message);

public slots:
    virtual bool startServer() = 0;
    virtual bool stopServer() = 0;
};

}

#endif // TRANSPORTINTERFACE_H
