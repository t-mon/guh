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

#include "authenticationmanager.h"
#include "loggingcategories.h"
#include "guhsettings.h"

#include <QCryptographicHash>
#include <QDateTime>

namespace guhserver {

AuthenticationManager::AuthenticationManager(QObject *parent) :
    QObject(parent)
{
    loadUsers();
    loadAuthorizedConnections();
}

QString AuthenticationManager::createToken()
{
    return QString(QCryptographicHash::hash(QUuid::createUuid().toByteArray(), QCryptographicHash::Sha3_256).toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}

QList<User> AuthenticationManager::users() const
{
    return m_users;
}

QList<AuthorizedConnection> AuthenticationManager::authorizedConnections() const
{
    return m_connections;
}

bool AuthenticationManager::verifyToken(const QString &token) const
{
    Q_UNUSED(token)

    return true;
}

bool AuthenticationManager::verifyLogin(const QString &userName, const QString &password)
{
    QByteArray encrypthedPassword = QCryptographicHash::hash((password.toUtf8()), QCryptographicHash::Sha3_512).toHex();

    if (!hasUser(userName)) {
        qCWarning(dcAuthentication) << "There is no user" << userName;
        return false;
    }

    foreach (const User &user, m_users) {
        if (user.userName() == userName && user.password().toUtf8() == encrypthedPassword) {
            return true;
        }
    }

    qCWarning(dcAuthentication) << "Wrong password for user" << userName;
    return false;
}

QString AuthenticationManager::authenticate(const QString &clientDescription, const QString &userName, const QString &password)
{
    bool loginVerfied = verifyLogin(userName, password);
    if (!loginVerfied)
        return QString();

    QString token = createToken();

    AuthorizedConnection connection;
    connection.setClientDescription(clientDescription);
    connection.setToken(token);
    connection.setLastLogin(QDateTime::currentMSecsSinceEpoch() / 1000);
    foreach (const User &user, m_users) {
        if (userName == user.userName()) {
            connection.setUser(user);
        }
    }

    m_connections.append(connection);
    saveAuthorizedConnections();

    return token;
}

bool AuthenticationManager::hasUser(const QString &userName)
{
    foreach (const User &user, m_users) {
        if (user.userName() == userName) {
            return true;
        }
    }
    return false;
}

void AuthenticationManager::loadUsers()
{

    GuhSettings settings(GuhSettings::SettingsRoleUsers);
    qCDebug(dcAuthentication) << "Loading user settings from" << settings.fileName();

    settings.beginGroup("Users");
    // create the default user if there are no users yet
    if (settings.childGroups().isEmpty()) {
        qCDebug(dcAuthentication) << "Creating default user \"guh\" with default password \"guh\".";
        settings.beginGroup(UserId::createUuid().toString());
        settings.setValue("username", "guh");
        settings.setValue("password", QString(QCryptographicHash::hash(("guh"), QCryptographicHash::Sha3_512).toHex()));
        settings.setValue("isAdmin", true);
        settings.endGroup();
    }

    foreach (const QString &userId, settings.childGroups()) {
        settings.beginGroup(userId);

        User user;
        user.setUserId(UserId(userId));
        user.setUserName(settings.value("username").toString());
        user.setPassword(settings.value("password").toString());
        user.setAdmin(settings.value("isAdmin").toBool());

        if (!user.isValid()) {
            qCWarning(dcAuthentication) << "Incomplete user data. Please check the";// << settings.fileName() << "file.";
            settings.endGroup();
            continue;
        }

        if (!hasUser(user.userName())) {
            m_users.append(user);
        } else {
            qCWarning(dcAuthentication) << "Duplicated username found. Not adding user" << user.userName() <<  "to the system.";
        }

        settings.endGroup();
    }

    if (!hasUser("guh")) {
        qCWarning(dcAuthentication) << "There is no default user configured. Please check your configuration";// << settings.fileName();
    }

    settings.endGroup();
}

void AuthenticationManager::saveUsers()
{
    GuhSettings settings(GuhSettings::SettingsRoleUsers);
    settings.beginGroup("Users");

    foreach (const User &user, m_users) {
        settings.beginGroup(user.userId().toString());
        settings.setValue("username", user.userName());
        settings.setValue("password", user.password());
        settings.setValue("isAdmin", user.isAdmin());
        settings.endGroup();
    }
    settings.endGroup();
}

void AuthenticationManager::loadAuthorizedConnections()
{

}

void AuthenticationManager::saveAuthorizedConnections()
{

}

}
