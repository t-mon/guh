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

AuthenticationManager::~AuthenticationManager()
{
    qCDebug(dcApplication) << "Shutting down \"Authentication Manager\"";
    saveAuthorizedConnections();
}

QList<User> AuthenticationManager::users() const
{
    return m_users;
}

QList<AuthorizedConnection> AuthenticationManager::authorizedConnections() const
{
    return m_connections;
}

bool AuthenticationManager::verifyToken(const QString &token)
{
    qCDebug(dcAuthentication) << "Verify token" << token;
    for (int i = 0; i < m_connections.count(); ++i) {
        if (m_connections.at(i).token() == token) {
            AuthorizedConnection newConnection(m_connections.at(i));
            newConnection.setLastLogin(QDateTime::currentDateTime().toTime_t());
            m_connections[i] = newConnection;
            qCDebug(dcAuthentication) << "Token authorized: last login" << QDateTime::fromTime_t(newConnection.lastLogin()).toString("dd.MM.yyyy hh:mm:ss");
            return true;
        }
    }

    qCWarning(dcAuthentication) << "Authentication failed. Token not authorized.";
    return false;
}

bool AuthenticationManager::verifyLogin(const QString &userName, const QString &password)
{
    if (!hasUser(userName)) {
        qCWarning(dcAuthentication) << "Authentication failed. There is no user" << userName;
        return false;
    }

    foreach (const User &user, m_users) {
        if (user.userName() == userName && user.password() == getPasswordHash(password)) {
            qCDebug(dcAuthentication) << "User" << userName << "authenticated successfully";
            return true;
        }
    }

    qCWarning(dcAuthentication) << "Authentication failed. Wrong password for user" << userName;
    return false;
}

bool AuthenticationManager::changePassword(const QString &userName, const QString &currentPassword, const QString &newPassword)
{
    bool loginVerfied = verifyLogin(userName, currentPassword);
    if (!loginVerfied) {
        qCWarning(dcAuthentication) << "Could not change password. Client authentication failed.";
        return false;
    }

    for (int i = 0; i < m_users.count(); i++) {
        if (m_users.at(i).userName() == userName) {
            User newUser(m_users.at(i));
            newUser.setPassword(getPasswordHash(newPassword));
            m_users[i] = newUser;
            qCWarning(dcAuthentication) << "Password for user" << userName << "changed successfully.";

            saveUsers();
            return true;
        }
    }
    return false;
}

QString AuthenticationManager::authenticate(const QString &clientDescription, const QString &userName, const QString &password)
{
    bool loginVerfied = verifyLogin(userName, password);
    if (!loginVerfied) {
        qCWarning(dcAuthentication) << "Client" << clientDescription << "authentication failed.";
        return QString();
    }

    QString token = createToken();
    AuthorizedConnection connection;
    connection.setClientDescription(clientDescription);
    connection.setToken(token);
    connection.setLastLogin(QDateTime::currentDateTime().toTime_t());
    foreach (const User &user, m_users) {
        if (userName == user.userName()) {
            connection.setUser(user);
        }
    }
    m_connections.append(connection);
    saveAuthorizedConnections();

    qCDebug(dcAuthentication) << "Client" << clientDescription << "authorized successfully. " << token;

    return token;
}

QString AuthenticationManager::createToken()
{
    return QString(QCryptographicHash::hash(QUuid::createUuid().toByteArray(), QCryptographicHash::Sha3_256).toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}

QString AuthenticationManager::getPasswordHash(const QString &password) const
{
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_256).toHex());
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
        settings.setValue("password", getPasswordHash("guh"));
        settings.setValue("isAdmin", true);
        settings.endGroup();
    }

#ifdef TESTING_ENABLED
    // add user for testing if not already added
    if (!settings.childGroups().contains("{17cf3358-fd30-4df9-a3b5-fe888cd70a24}")) {
        settings.beginGroup("{17cf3358-fd30-4df9-a3b5-fe888cd70a24}");
        settings.setValue("username", "guh-test");
        settings.setValue("password", getPasswordHash("guh_test_password@1234"));
        settings.setValue("isAdmin", true);
        settings.endGroup();
    }
#endif

    foreach (const QString &userId, settings.childGroups()) {
        settings.beginGroup(userId);

        User user;
        user.setUserId(UserId(userId));
        user.setUserName(settings.value("username").toString());
        user.setPassword(settings.value("password").toString());
        user.setAdmin(settings.value("isAdmin").toBool());

        if (!user.isValid()) {
            qCWarning(dcAuthentication) << "Incomplete user data. Please check the" << settings.fileName() << "file.";
            settings.endGroup();
            continue;
        }

        if (hasUser(user.userName())) {
            qCWarning(dcAuthentication) << "Duplicated username found. Not adding user" << user.userName() <<  "to the system.";
            settings.endGroup();
            continue;
        }

        m_users.append(user);
        settings.endGroup();
    }

    if (!hasUser("guh"))
        qCWarning(dcAuthentication) << "There is no default user \"guh\" configured. Please check your configuration" << settings.fileName();

    settings.endGroup();
}

void AuthenticationManager::saveUsers()
{
    GuhSettings settings(GuhSettings::SettingsRoleUsers);
    qCDebug(dcAuthentication) << "Loading users from" << settings.fileName();

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
    GuhSettings settings(GuhSettings::SettingsRoleUsers);
    qCDebug(dcAuthentication) << "Loading authorized connections from" << settings.fileName();

    settings.beginGroup("AuthorizedConnections");

#ifdef TESTING_ENABLED
    // add authorized connection for testing
    settings.beginGroup("4VzMAR3PozoPKkwCb0x0-pBWESwdL5YdRGbJn4I9TRE");
    settings.setValue("clientDescription", "guh-tests");
    settings.setValue("userId", "{17cf3358-fd30-4df9-a3b5-fe888cd70a24}");
    settings.setValue("lastLogin", QDateTime::currentDateTime().toTime_t());
    settings.endGroup();
#endif

    foreach (const QString &token, settings.childGroups()) {
        settings.beginGroup(token);
        AuthorizedConnection connection;
        connection.setClientDescription(settings.value("clientDescription").toString());
        connection.setToken(token);
        connection.setLastLogin((quint64)settings.value("lastLogin").toUInt());
        foreach (const User &user, m_users) {
            if (UserId(settings.value("userId").toString()) == user.userId()) {
                connection.setUser(user);
            }
        }
        m_connections.append(connection);
        settings.endGroup();
    }
    settings.endGroup();
}

void AuthenticationManager::saveAuthorizedConnections()
{
    GuhSettings settings(GuhSettings::SettingsRoleUsers);
    settings.beginGroup("AuthorizedConnections");

    foreach (const AuthorizedConnection &connection, m_connections) {
        settings.beginGroup(connection.token());
        settings.setValue("clientDescription", connection.clientDescription());
        settings.setValue("userId", connection.user().userId().toString());
        settings.setValue("lastLogin", connection.lastLogin());
        settings.endGroup();
    }
    settings.endGroup();
}

}
