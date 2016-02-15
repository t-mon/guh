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

/*!
    \class guhserver::AuthenticationManager
    \brief Manager for all API authentications and authorized connection.

    \inmodule core

    The AuthenticationManager takes care of all API authentications, users and connections.

    \sa User, AuthorizedConnection
*/

/*! \enum guhserver::AuthenticationManager::AuthenticationError

    \value AuthenticationErrorNoError
        No error happened. Everything is fine.
    \value AuthenticationErrorAuthenicationFailed
        The given username, password or token is not valid.
    \value AuthenticationErrorWeakPassword
        The given password is to weak. The user has to take a stronger password.
    \value AuthenticationErrorPermissionDenied
        The authentication was valid, but this user has not the needed permission.
    \value AuthenticationErrorAuthenticationDisabled
        The authentication API is disabled.
*/

/*! \fn void guhserver::AuthenticationManager::authorizedConnectionAdded(const AuthorizedConnection &connection);
    Will be emitted whenever an \l{AuthorizedConnection} was added to the system.
    The parameter \a connection holds the new \l{AuthorizedConnection}.
*/

/*! \fn void guhserver::AuthenticationManager::authorizedConnectionRemoved(const AuthorizedConnection &connection);
    Will be emitted whenever an \l{AuthorizedConnection} was removed from the system.
    The parameter \a connection holds the removed \l{AuthorizedConnection}.
*/


#include "authenticationmanager.h"
#include "loggingcategories.h"
#include "guhsettings.h"

#include <QCryptographicHash>
#include <QDateTime>

namespace guhserver {

/*! Constructs an AuthenticationManager with the given \a parent. */
AuthenticationManager::AuthenticationManager(QObject *parent) :
    QObject(parent)
{
#ifdef TESTING_ENABLED
    // Note: if you change this, change the values also in the testbase
    m_testUserName = "guh-test";
    m_testUserPassword = "guh_test_password@1234";
    m_testUserId = UserId("17cf3358-fd30-4df9-a3b5-fe888cd70a24");
    m_testToken = "4VzMAR3PozoPKkwCb0x0-pBWESwdL5YdRGbJn4I9TRE";
#endif

    loadUsers();
    loadAuthorizedConnections();
}

/*! Destructs this AuthenticationManager. */
AuthenticationManager::~AuthenticationManager()
{
    qCDebug(dcApplication) << "Shutting down \"Authentication Manager\"";
    saveAuthorizedConnections();
}

/*! Returns the list of available \l{User}{Users}. */
QList<User> AuthenticationManager::users() const
{
    return m_users;
}

/*! Returns the \l{User} with the given \a userId. */
User AuthenticationManager::getUser(const UserId &userId) const
{
    foreach (const User &user, m_users) {
        if (user.userId() == userId) {
            return user;
        }
    }
    return User();
}

/*! Returns the \l{User} with the given \a userName. */
User AuthenticationManager::getUser(const QString &userName) const
{
    foreach (const User &user, m_users) {
        if (user.userName() == userName) {
            return user;
        }
    }
    return User();
}

/*! Returns true if the \a currentPassword of the \l{User} with the given \a userName
 * could be changed to the \a newPassword. */
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

/*! Returns the list of authorized connections.

    \sa AuthorizedConnection
*/
QList<AuthorizedConnection> AuthenticationManager::authorizedConnections() const
{
    return m_connections;
}

/*! Returns the list of authorized connections for the given \a userId.

    \sa AuthorizedConnection
*/
QList<AuthorizedConnection> AuthenticationManager::getAuthorizedConnections(const UserId &userId) const
{
    QList<AuthorizedConnection> connections;
    foreach (const AuthorizedConnection &connection, m_connections) {
        if (connection.userId() == userId) {
            connections.append(connection);
        }
    }
    return connections;
}

/*! Removes the given \a tokenList from the list of authorized connections. */
void AuthenticationManager::removeAuthorizedConnections(const QList<QString> tokenList)
{
    GuhSettings settings(GuhSettings::SettingsRoleUsers);
    settings.beginGroup("AuthorizedConnections");

    foreach (const QString &token, tokenList) {
        for (int i = 0; i < m_connections.count(); i++) {
            if (m_connections.at(i).token() == token) {
                qCDebug(dcAuthentication) << "Remove authorized connection" << m_connections.at(i).clientDescription() << token;
                settings.beginGroup(token);
                settings.remove("");
                settings.endGroup();
                emit authorizedConnectionRemoved( m_connections.at(i));
                m_connections.removeAt(i);
            }
        }
    }
}

/*! Returns true, if the given \a password is strong enough. */
bool AuthenticationManager::verifyPasswordStrength(const QString &password) const
{
    if (password.isEmpty()) {
        qCWarning(dcAuthentication) << "Password to weak. Empty password forbidden.";
        return false;
    }

    if (password.length() < 5) {
        qCWarning(dcAuthentication) << "Password to weak. The password is to short.";
        return false;
    }

    return true;
}

/*! Returns true, if the given \a token is authorized. */
bool AuthenticationManager::verifyToken(const QString &token)
{
    qCDebug(dcAuthentication) << "Verify token" << token;
    for (int i = 0; i < m_connections.count(); ++i) {
        if (m_connections.at(i).token() == token) {
            AuthorizedConnection newConnection(m_connections.at(i));
            newConnection.setLastLogin(QDateTime::currentDateTime().toTime_t());
            m_connections[i] = newConnection;
            qCDebug(dcAuthentication) << "Token authorized for user" << getUser(newConnection.userId()).userName() << ": last login" << QDateTime::fromTime_t(newConnection.lastLogin()).toString("dd.MM.yyyy hh:mm:ss");
            return true;
        }
    }

    qCWarning(dcAuthentication) << "Authentication failed. Token not authorized.";
    return false;
}

/*! Returns true, if the given \a userName and \a password are valid. */
bool AuthenticationManager::verifyLogin(const QString &userName, const QString &password)
{
    if (!userExists(userName)) {
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

/*! Returns a new generated token, if the given \a userName and \a password for the client with the given \a clientDescription are authenticated successfully.
 Otherwise returns QString(). */
QString AuthenticationManager::authenticate(const QString &clientDescription, const QString &userName, const QString &password)
{
    bool loginVerfied = verifyLogin(userName, password);
    if (!loginVerfied) {
        qCWarning(dcAuthentication) << "Client" << userName << "from" << clientDescription << "authentication failed.";
        return QString();
    }

    QString token = createToken();

    AuthorizedConnection connection;
    connection.setClientDescription(clientDescription);
    connection.setToken(token);
    connection.setLastLogin(QDateTime::currentDateTime().toTime_t());
    connection.setUserId(getUser(userName).userId());

    qCDebug(dcAuthentication) << "Client" << clientDescription << "authorized successfully. " << token;
    m_connections.append(connection);
    saveAuthorizedConnections();

    emit authorizedConnectionAdded(connection);

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

bool AuthenticationManager::userExists(const QString &userName)
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
    qCDebug(dcAuthentication) << "Loading users from" << settings.fileName();

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
    if (!settings.childGroups().contains(m_testUserId.toString())) {
        settings.beginGroup(m_testUserId.toString());
        settings.setValue("username", m_testUserName);
        settings.setValue("password", getPasswordHash(m_testUserPassword));
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

        if (userExists(user.userName())) {
            qCWarning(dcAuthentication) << "Duplicated username found. Not adding user" << user.userName() << user.userId().toString() <<  "to guh.";
            settings.endGroup();
            continue;
        }

        m_users.append(user);
        settings.endGroup();
    }

    if (!userExists("guh"))
        qCWarning(dcAuthentication) << "There is no default user \"guh\" configured. Please check your configuration" << settings.fileName();

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
    GuhSettings settings(GuhSettings::SettingsRoleUsers);
    qCDebug(dcAuthentication) << "Loading authorized connections from" << settings.fileName();

    settings.beginGroup("AuthorizedConnections");

#ifdef TESTING_ENABLED
    // add authorized connection for testing
    settings.beginGroup(m_testToken);
    settings.setValue("clientDescription", "guh-tests");
    settings.setValue("userId", m_testUserId.toString());
    settings.setValue("lastLogin", QDateTime::currentDateTime().toTime_t());
    settings.endGroup();
#endif

    foreach (const QString &token, settings.childGroups()) {
        settings.beginGroup(token);
        AuthorizedConnection connection;
        connection.setClientDescription(settings.value("clientDescription").toString());
        connection.setToken(token);
        connection.setLastLogin((quint64)settings.value("lastLogin").toUInt());
        connection.setUserId(UserId(settings.value("userId").toString()));
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
        settings.setValue("userId", connection.userId().toString());
        settings.setValue("lastLogin", connection.lastLogin());
        settings.endGroup();
    }
    settings.endGroup();
}

}
