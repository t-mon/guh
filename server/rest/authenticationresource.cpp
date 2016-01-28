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

guhserver::HttpReply *guhserver::AuthenticationResource::proccessRequest(const guhserver::HttpRequest &request, const QStringList &urlTokens)
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
//    case HttpRequest::Put:
//        reply = proccessPutRequest(request, urlTokens);
//        break;
//    case HttpRequest::Delete:
//        reply = proccessDeleteRequest(request, urlTokens);
//        break;
//    case HttpRequest::Options:
//        reply = proccessOptionsRequest(request, urlTokens);
//        break;
    default:
        reply = createErrorReply(HttpReply::BadRequest);
        break;
    }

    return reply;
}

HttpReply *AuthenticationResource::proccessGetRequest(const HttpRequest &request, const QStringList &urlTokens)
{
    // GET /api/v1/authentication
    if (urlTokens.count() == 3)
        return createErrorReply(HttpReply::NotImplemented);

    // GET /api/v1/authentication/login
    if (urlTokens.count() == 4 && urlTokens.at(3) == "login")
        return getLogin(request);

//    // GET /api/v1/authentication/authorizedconnections
//    if (urlTokens.count() == 4 && urlTokens.at(3) == "authorizedconnections")
//        return getAuthorizedConnections(request);


    return createErrorReply(HttpReply::NotImplemented);
}

guhserver::HttpReply *guhserver::AuthenticationResource::proccessPostRequest(const guhserver::HttpRequest &request, const QStringList &urlTokens)
{
    Q_UNUSED(request)
    Q_UNUSED(urlTokens)

    return createSuccessReply();
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

//HttpReply *AuthenticationResource::getAuthorizedConnections(const HttpRequest &request) const
//{

//}

}
