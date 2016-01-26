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
        AuthenticationErrorWeakPassword,
        AuthenticationErrorPermissionDenied
    };

    explicit AuthenticationManager(QObject *parent = 0);
    ~AuthenticationManager();

    // users
    QList<User> users() const;
    User getUser(const UserId &userId) const;
    User getUser(const QString &userName) const;
    bool changePassword(const QString &userName, const QString &currentPassword, const QString &newPassword);

    // Authorized connections
    QList<AuthorizedConnection> authorizedConnections() const;
    QList<AuthorizedConnection> getAuthorizedConnections(const UserId &userId) const;
    void removeAuthorizedConnections(const QList<QString> tokenList);
    bool verifyPasswordStrength(const QString &password) const;

    // access methods
    bool verifyToken(const QString &token);
    bool verifyLogin(const QString &userName, const QString &password);
    QString authenticate(const QString &clientDescription, const QString &userName, const QString &password);

private:
    QList<User> m_users;
    QList<AuthorizedConnection> m_connections;

#ifdef TESTING_ENABLED
    // used for testing
    UserId m_testUserId;
    QString m_testUserName;
    QString m_testUserPassword;
    QString m_testToken;
#endif

    // create token/password
    QString createToken();
    QString getPasswordHash(const QString &password) const;

    bool userExists(const QString &userName);

    void loadUsers();
    void saveUsers();

    void loadAuthorizedConnections();
    void saveAuthorizedConnections();

signals:
    void authorizedConnectionAdded(const AuthorizedConnection &connection);
    void authorizedConnectionRemoved(const AuthorizedConnection &connection);

};

}

Q_DECLARE_METATYPE(guhserver::AuthenticationManager::AuthenticationError)

#endif // AUTHENTICATIONMANAGER_H
