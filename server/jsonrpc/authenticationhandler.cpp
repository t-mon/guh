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
    setDescription("Authenticate", "This method can be called to get an authentication token.");
    params.insert("clientDescription", JsonTypes::basicTypeToString(JsonTypes::String));
    params.insert("userName", JsonTypes::basicTypeToString(JsonTypes::String));
    params.insert("password", JsonTypes::basicTypeToString(JsonTypes::String));
    setParams("Authenticate", params);
    returns.insert("o:token", JsonTypes::basicTypeToString(JsonTypes::String));
    returns.insert("authenticationError", JsonTypes::authenticationErrorRef());
    setReturns("Authenticate", returns);
}

QString AuthenticationHandler::name() const
{
    return "Authentication";
}

JsonReply *AuthenticationHandler::Authenticate(const QVariantMap &params)
{
    qCDebug(dcJsonRpc) << params;

    QString clientDescription = params.value("clientDescription").toString();
    QString userName = params.value("userName").toString();
    QString password = params.value("password").toString();

    QString token = GuhCore::instance()->authenticationManager()->authenticate(clientDescription, userName, password);

    if (token.isEmpty())
        return createReply(statusToReply(AuthenticationManager::AuthenticationErrorAuthenicationFailed));

    QVariantMap returnParams;
    returnParams.insert("token", GuhCore::instance()->authenticationManager()->createToken());
    returnParams.insert("authenticationError", JsonTypes::authenticationErrorToString(AuthenticationManager::AuthenticationErrorNoError));
    return createReply(returnParams);
}

}
