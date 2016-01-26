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
//    case HttpRequest::Get:
//        reply = proccessGetRequest(request, urlTokens);
//        break;
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

guhserver::HttpReply *guhserver::AuthenticationResource::proccessPostRequest(const guhserver::HttpRequest &request, const QStringList &urlTokens)
{
    Q_UNUSED(request)
    Q_UNUSED(urlTokens)

    return createSuccessReply();
}

}
