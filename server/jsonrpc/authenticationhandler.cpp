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

#include "authenticationhandler.h"
#include "loggingcategories.h"
#include "guhcore.h"

namespace guhserver {

AuthenticationHandler::AuthenticationHandler(QObject *parent) :
    JsonHandler(parent)
{
    QVariantMap params;
    QVariantMap returns;

    params.clear(); returns.clear();
    setDescription("Authenticate", "This method can be called to get an authentication token. "
                   "If the authentication was successfull, the access token will be returned.");
    params.insert("clientDescription", JsonTypes::basicTypeToString(JsonTypes::String));
    params.insert("userName", JsonTypes::basicTypeToString(JsonTypes::String));
    params.insert("password", JsonTypes::basicTypeToString(JsonTypes::String));
    setParams("Authenticate", params);
    returns.insert("o:token", JsonTypes::basicTypeToString(JsonTypes::String));
    returns.insert("authenticationError", JsonTypes::authenticationErrorRef());
    setReturns("Authenticate", returns);


    params.clear(); returns.clear();
    setDescription("ChangePassword", "This method can be called to change the password of the given user name.");
    params.insert("userName", JsonTypes::basicTypeToString(JsonTypes::String));
    params.insert("password", JsonTypes::basicTypeToString(JsonTypes::String));
    params.insert("newPassword", JsonTypes::basicTypeToString(JsonTypes::String));
    setParams("ChangePassword", params);
    returns.insert("authenticationError", JsonTypes::authenticationErrorRef());
    setReturns("ChangePassword", returns);

    params.clear(); returns.clear();
    setDescription("GetAuthorizedConnections", "This method returns the list of authorized connection in guh. "
                   "Each connection represents an access token.");
    params.insert("o:userId", JsonTypes::basicTypeToString(JsonTypes::Uuid));
    setParams("GetAuthorizedConnections", params);
    returns.insert("o:connections", QVariantList() << JsonTypes::authorizedConnectionRef());
    returns.insert("authenticationError", JsonTypes::authenticationErrorRef());
    setReturns("GetAuthorizedConnections", returns);

    params.clear(); returns.clear();
    setDescription("RemoveAuthorizedConnections", "This method allows to remove authorized connections from the system. "
                   "The connection will be represented by the token. This forces the client to re-authenticate.");
    params.insert("tokens", QVariantList() << JsonTypes::basicTypeToString(JsonTypes::String));
    setParams("RemoveAuthorizedConnections", params);
    returns.insert("authenticationError", JsonTypes::authenticationErrorRef());
    setReturns("RemoveAuthorizedConnections", returns);
}

QString AuthenticationHandler::name() const
{
    return "Authentication";
}

JsonReply *AuthenticationHandler::Authenticate(const QVariantMap &params)
{
    QString clientDescription = params.value("clientDescription").toString();
    QString userName = params.value("userName").toString();
    QString password = params.value("password").toString();

    QString token = GuhCore::instance()->authenticationManager()->authenticate(clientDescription, userName, password);
    if (token.isEmpty())
        return createReply(statusToReply(AuthenticationManager::AuthenticationErrorAuthenicationFailed));

    QVariantMap returnParams;
    returnParams.insert("token", token);
    returnParams.insert("authenticationError", JsonTypes::authenticationErrorToString(AuthenticationManager::AuthenticationErrorNoError));
    return createReply(returnParams);
}

JsonReply *AuthenticationHandler::ChangePassword(const QVariantMap &params)
{
    QString userName = params.value("userName").toString();
    QString currentPassword = params.value("password").toString();
    QString newPassword = params.value("newPassword").toString();

    if (!GuhCore::instance()->authenticationManager()->verifyPasswordStrength(newPassword))
        return createReply(statusToReply(AuthenticationManager::AuthenticationErrorWeakPassword));

    if (!GuhCore::instance()->authenticationManager()->changePassword(userName, currentPassword, newPassword))
        return createReply(statusToReply(AuthenticationManager::AuthenticationErrorAuthenicationFailed));

    return createReply(statusToReply(AuthenticationManager::AuthenticationErrorNoError));
}

JsonReply *AuthenticationHandler::GetAuthorizedConnections(const QVariantMap &params)
{
    Q_UNUSED(params)
    QVariantList connectionList;
    foreach (const AuthorizedConnection &connection, GuhCore::instance()->authenticationManager()->authorizedConnections()) {
        connectionList.append(JsonTypes::packAuthorizedConnection(connection));
    }

    QVariantMap returnParams;
    returnParams.insert("connections", connectionList);
    returnParams.insert("authenticationError", JsonTypes::authenticationErrorToString(AuthenticationManager::AuthenticationErrorNoError));
    return createReply(returnParams);
}

JsonReply *AuthenticationHandler::RemoveAuthorizedConnections(const QVariantMap &params)
{
    qCDebug(dcJsonRpc) << "Remove authorized connections with tokens" << params;
    QList<QString> tokenList = params.value("tokens").toStringList();
    GuhCore::instance()->authenticationManager()->removeAuthorizedConnections(tokenList);
    return createReply(statusToReply(AuthenticationManager::AuthenticationErrorNoError));
}

}
