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

#include "authenticationresource.h"
#include "loggingcategories.h"
#include "guhcore.h"

#include <QJsonDocument>

namespace guhserver {

AuthenticationResource::AuthenticationResource(QObject *parent) :
    RestResource(parent)
{

}

QString AuthenticationResource::name() const
{
    return "authentication";
}

guhserver::HttpReply *guhserver::AuthenticationResource::proccessRequest(const HttpRequest &request, const QStringList &urlTokens)
{
    // check method
    HttpReply *reply;
    switch (request.method()) {
    case HttpRequest::Get:
        reply = proccessGetRequest(request, urlTokens);
        break;
    case HttpRequest::Post:
        reply = proccessPostRequest(request, urlTokens);
        break;
    case HttpRequest::Put:
        reply = proccessPutRequest(request, urlTokens);
        break;
    case HttpRequest::Options:
        reply = proccessOptionsRequest(request, urlTokens);
        break;
    default:
        reply = createErrorReply(HttpReply::BadRequest);
        break;
    }

    return reply;
}

HttpReply *AuthenticationResource::proccessGetRequest(const HttpRequest &request, const QStringList &urlTokens)
{
    // GET /api/v1/authentication/login
    if (urlTokens.count() == 4 && urlTokens.at(3) == "login")
        return getLogin(request);

    // GET /api/v1/authentication/authorizedconnections
    if (urlTokens.count() == 4 && urlTokens.at(3) == "authorizedconnections")
        return getAuthorizedConnections(request);

    return createErrorReply(HttpReply::NotImplemented);
}

guhserver::HttpReply *guhserver::AuthenticationResource::proccessPutRequest(const HttpRequest &request, const QStringList &urlTokens)
{
    // PUT /api/v1/authentication/changepassword
    if (urlTokens.count() == 4 && urlTokens.at(3) == "changepassword")
        return changePassword(request);

    return createErrorReply(HttpReply::NotImplemented);
}

HttpReply *AuthenticationResource::proccessPostRequest(const HttpRequest &request, const QStringList &urlTokens)
{
    // POST /api/v1/authentication/removeconnections
    if (urlTokens.count() == 4 && urlTokens.at(3) == "removeconnections")
        return removeConnections(request);

    return createErrorReply(HttpReply::NotImplemented);
}

HttpReply *AuthenticationResource::proccessOptionsRequest(const HttpRequest &request, const QStringList &urlTokens)
{
    Q_UNUSED(request)
    Q_UNUSED(urlTokens)
    return RestResource::createCorsSuccessReply();
}

HttpReply *AuthenticationResource::getLogin(const HttpRequest &request) const
{
    if (!request.rawHeaderList().keys().contains("Authorization")) {
        qCWarning(dcRest) << "Authorization header missing.";
        return createErrorReply(HttpReply::BadRequest);
    }

    QList<QByteArray> authorizationParts = request.rawHeaderList().value("Authorization").split(' ');
    if (authorizationParts.count() != 2) {
        qCWarning(dcRest) << "Invalid Authorization header format.";
        return createErrorReply(HttpReply::BadRequest);
    }

    if (authorizationParts.first() != "Basic") {
        qCWarning(dcRest) << "Invalid Authorization header format.";
        return createErrorReply(HttpReply::BadRequest);
    }

    // Note: base64 has no character ':', so it should alway be splited correctly if the token is authorized...
    QStringList loginParts = QString(QByteArray::fromBase64(authorizationParts.last())).split(':');

    if (loginParts.count() != 2) {
        qCWarning(dcRest) << "Invalid login format.";
        return createErrorReply(HttpReply::BadRequest);
    }

    // authenticate and get token
    QString token = GuhCore::instance()->authenticationManager()->authenticate(request.userAgent(), loginParts.first(), loginParts.last());
    if (token.isEmpty())
        return createAuthenticationErrorReply(HttpReply::Unauthorized, AuthenticationManager::AuthenticationErrorAuthenicationFailed);

    QVariantMap params;
    params.insert("token", token);

    HttpReply *reply = createSuccessReply();
    reply->setPayload(QJsonDocument::fromVariant(params).toJson());
    return reply;
}

HttpReply *AuthenticationResource::getAuthorizedConnections(const HttpRequest &request) const
{
    Q_UNUSED(request)

    // TODO: parse optional userId

    qCDebug(dcRest) << "Get authorized connections";
    QVariantList connectionList;
    foreach (const AuthorizedConnection &connection, GuhCore::instance()->authenticationManager()->authorizedConnections()) {
        connectionList.append(JsonTypes::packAuthorizedConnection(connection));
    }

    HttpReply *reply = createSuccessReply();
    reply->setPayload(QJsonDocument::fromVariant(connectionList).toJson());
    return reply;
}

HttpReply *AuthenticationResource::changePassword(const HttpRequest &request) const
{
    qCDebug(dcRest) << "Change password";
    QPair<bool, QVariant> verification = RestResource::verifyPayload(request.payload());
    if (!verification.first)
        return createErrorReply(HttpReply::BadRequest);

    QVariantMap params = verification.second.toMap();

    // TODO: check permissions if this user is allowed to change the password of the given username

    QString userName = params.value("userName").toString();
    QString currentPassword = params.value("password").toString();
    QString newPassword = params.value("newPassword").toString();

    if (!GuhCore::instance()->authenticationManager()->verifyPasswordStrength(newPassword))
        return createAuthenticationErrorReply(HttpReply::BadRequest, AuthenticationManager::AuthenticationErrorWeakPassword);

    if (!GuhCore::instance()->authenticationManager()->changePassword(userName, currentPassword, newPassword))
        return createAuthenticationErrorReply(HttpReply::Unauthorized, AuthenticationManager::AuthenticationErrorAuthenicationFailed);

    return createSuccessReply();
}

HttpReply *AuthenticationResource::removeConnections(const HttpRequest &request) const
{
    qCDebug(dcRest) << "Remove authorized connections";
    QPair<bool, QVariant> verification = RestResource::verifyPayload(request.payload());
    if (!verification.first)
        return createErrorReply(HttpReply::BadRequest);

    QStringList tokenList = verification.second.toMap().value("tokens").toStringList();
    GuhCore::instance()->authenticationManager()->removeAuthorizedConnections(tokenList);

    return createSuccessReply();
}

}
