/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2016 Simon Stuerz <simon.stuerz@guh.guru>                *
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

#ifndef AUTHENTICATIONMANAGER_H
#define AUTHENTICATIONMANAGER_H

#include <QObject>
#include <QAuthenticator>

#include "authentication/user.h"
#include "authentication/authorizedconnection.h"

namespace guhserver {

class AuthenticationManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(AuthenticationError)

public:
    enum AuthenticationError {
        AuthenticationErrorNoError,
        AuthenticationErrorAuthenicationFailed,
        AuthenticationErrorPermissionDenied
    };

    explicit AuthenticationManager(QObject *parent = 0);

    static QString createToken();

    QList<User> users() const;
    QList<AuthorizedConnection> authorizedConnections() const;

    bool verifyToken(const QString &token) const;
    bool verifyLogin(const QString &userName, const QString &password);

    QString authenticate(const QString &clientDescription, const QString &userName, const QString &password);

private:
    QList<User> m_users;
    QList<AuthorizedConnection> m_connections;

    bool hasUser(const QString &userName);

    void loadUsers();
    void saveUsers();
    void loadAuthorizedConnections();
    void saveAuthorizedConnections();

};

}
#endif // AUTHENTICATIONMANAGER_H
