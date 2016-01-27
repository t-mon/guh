/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2015 Simon Stuerz <simon.stuerz@guh.guru>                *
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
  \class guhserver::HttpRequest
  \brief Represents a HTTP request from a client to the guh \l{WebServer}.

  \ingroup api
  \inmodule core

  This class holds the header and the payload data of a network request from a client to the \l{WebServer}.

  \note RFC 7231 HTTP/1.1 Semantics and Content -> \l{http://tools.ietf.org/html/rfc7231}{http://tools.ietf.org/html/rfc7231}

*/

/*! \enum guhserver::HttpRequest::RequestMethod

    This enum type describes the method of a \l{HttpRequest}. Following methods are allowed/handled:

    \value Get
        Represents the HTTP/1.1 GET method.
    \value Post
        Represents the HTTP/1.1 POST method.
    \value Put
        Represents the HTTP/1.1 PUT method.
    \value Delete
        Represents the HTTP/1.1 DELETE method.
    \value Options
        Represents the HTTP/1.1 OPTIONS method.
    \value Unhandled
        Represents every other method which is not handled.
*/

/*! \fn QDebug guhserver::operator<< (QDebug debug, const HttpRequest &httpRequest);
    Writes the \l{HttpRequest} \a httpRequest to the given \a debug. This method gets used just for debugging.
*/

#include "httprequest.h"
#include "loggingcategories.h"

#include <QUrlQuery>

namespace guhserver {

/*! Construct an empty \l{HttpRequest}. */
HttpRequest::HttpRequest() :
    m_rawData(QByteArray()),
    m_valid(false),
    m_isComplete(false)
{
}

/*! Construct a \l{HttpRequest} with the given \a rawData. The \a rawData will be parsed in this constructor. You can check
    if the data is valid with \l{isValid()}. You can check if the request is complete with \l{isComplete}.

    \sa isValid(), isComplete()
*/
HttpRequest::HttpRequest(QByteArray rawData) :
    m_rawData(rawData),
    m_valid(false),
    m_isComplete(false)
{
    validate();
}

/*! Returns the raw header of this request.*/
QByteArray HttpRequest::rawHeader() const
{
    return m_rawHeader;
}

/*! Returns the list of raw header splitted into key and value.*/
QHash<QByteArray, QByteArray> HttpRequest::rawHeaderList() const
{
    return m_rawHeaderList;
}

/*! Returns the \l{RequestMethod} of this request.

  \sa RequestMethod
*/
HttpRequest::RequestMethod HttpRequest::method() const
{
    return m_method;
}

/*! Returns the method as human readable string.*/
QString HttpRequest::methodString() const
{
    return m_methodString;
}

/*! Returns the HTTP version of this \l{HttpRequest}.*/
QByteArray HttpRequest::httpVersion() const
{
    return m_httpVersion;
}

QString HttpRequest::userAgent() const
{
    return m_userAgent;
}

/*! Returns the URL of this \l{HttpRequest}.*/
QUrl HttpRequest::url() const
{
    return m_url;
}

/*! Returns the URL query of this \l{HttpRequest}.*/
QUrlQuery HttpRequest::urlQuery() const
{
    return m_urlQuery;
}

/*! Returns the payload (content) of this \l{HttpRequest}.*/
QByteArray HttpRequest::payload() const
{
    return m_payload;
}

/*! Returns true if this \l{HttpRequest} is valid. A HTTP request is valid if the header and the payload were paresed successfully without errors.*/
bool HttpRequest::isValid() const
{
    return m_valid;
}

/*! Returns true if this \l{HttpRequest} is complete. A HTTP request is complete if "Content-Length" header value matches the actual payload size. Bigger packages will be sent in multiple TCP packages. */
bool HttpRequest::isComplete() const
{
    return m_isComplete;
}

/*! Returns true if this \l{HttpRequest} has a payload.*/
bool HttpRequest::hasPayload() const
{
    return !m_payload.isEmpty();
}

/*! Appends the given \a data to the current raw data of this \l{HttpRequest}.
 *  This method will be used if a \l{HttpRequest} is not complete yet.
 *
 *  \sa isComplete()
*/
void HttpRequest::appendData(const QByteArray &data)
{
    m_rawData.append(data);
    validate();
}

void HttpRequest::validate()
{
    m_isComplete = true; m_valid = false;

    // Parese the HTTP request. The request is invalid, until the end of the parse process.
    if (m_rawData.isEmpty())
        return;

    // split the data into header and payload
    int headerEndIndex = m_rawData.indexOf("\r\n\r\n");
    if (headerEndIndex < 0) {
        qCWarning(dcWebServer) << "Could not parse end of HTTP header (empty line between header and body):" << m_rawData;
        return;
    }

    m_rawHeader = m_rawData.left(headerEndIndex);
    m_payload = m_rawData.right(m_rawData.length() - headerEndIndex).simplified();

    // parse status line
    QStringList headerLines = QString(m_rawHeader).split(QRegExp("\r\n"));
    QString statusLine = headerLines.takeFirst();
    QStringList statusLineTokens = statusLine.split(QRegExp("[ \r\n][ \r\n]*"));
    if (statusLineTokens.count() != 3) {
        qCWarning(dcWebServer) << "Could not parse HTTP status line:" << statusLine;
        return;
    }

    // verify http version
    m_httpVersion = statusLineTokens.at(2).toUtf8().simplified();
    if (!m_httpVersion.contains("HTTP")) {
        qCWarning(dcWebServer) << "Unknown HTTP version:" << m_httpVersion;
        return;
    }
    m_methodString = statusLineTokens.at(0).simplified();
    m_method = getRequestMethodType(m_methodString);

    m_url = QUrl("http://example.com" + statusLineTokens.at(1).simplified());

    if (m_url.hasQuery())
        m_urlQuery = QUrlQuery(m_url.query());

    // verify header formating
    foreach (const QString &line, headerLines) {
        if (!line.contains(":")) {
            qCWarning(dcWebServer) << "Invalid HTTP header:" << line;
            return;
        }
        int index = line.indexOf(":");
        QByteArray key = line.left(index).toUtf8().simplified();
        QByteArray value = line.right(line.count() - index - 1).toUtf8().simplified();
        m_rawHeaderList.insert(key, value);
    }

    // check User-Agent
    if (!m_rawHeaderList.contains("User-Agent")) {
        qWarning() << "User-Agent header is missing";
        return;
    }

    m_userAgent = QString(m_rawHeaderList.value("User-Agent"));

    // verify content length with actual payload
    if (m_rawHeaderList.contains("Content-Length")) {
        bool ok = false;
        int contentLength = m_rawHeaderList.value("Content-Length").toInt(&ok);
        if (!ok) {
            qCWarning(dcWebServer) << "Could not parse Content-Length.";
            return;
        }
        // check if we have all data
        if (m_payload.size() < contentLength) {
            qCDebug(dcWebServer) << "Request incomplete:";
            qCDebug(dcWebServer) << "   -> Content-Length:" << contentLength;
            qCDebug(dcWebServer) << "   -> Payload size  :" << payload().size();
            m_isComplete = false;
            return;
        }
        // check if the content lenght bigger than header Content-Length
        if (m_payload.size() > contentLength) {
            qCWarning(dcWebServer) << "Payload size greater than header Content-Length:";
            qCWarning(dcWebServer) << "   -> Content-Length:" << contentLength;
            qCWarning(dcWebServer) << "   -> Payload size  :" << payload().size();
            m_isComplete = true;
            return;
        }

    }
    m_valid = true;
}

HttpRequest::RequestMethod HttpRequest::getRequestMethodType(const QString &methodString)
{
    if (methodString == "GET") {
        return RequestMethod::Get;
    } else if (methodString == "POST") {
        return RequestMethod::Post;
    } else if (methodString == "PUT") {
        return RequestMethod::Put;
    } else if (methodString == "DELETE") {
        return RequestMethod::Delete;
    } else if (methodString == "OPTIONS") {
        return RequestMethod::Options;
    }
    qCWarning(dcWebServer) << "Method" << methodString << "will not be handled.";
    return RequestMethod::Unhandled;
}

QDebug operator<<(QDebug debug, const HttpRequest &httpRequest)
{
    debug << "===================================" << "\n";
    debug << "  HTTP version: " << httpRequest.httpVersion() << "\n";
    debug << "        method: " << httpRequest.methodString() << "\n";
    debug << "      URL path: " << httpRequest.url().path() << "\n";
    debug << "     URL query: " << httpRequest.urlQuery().query() << "\n";
    debug << "      is valid: " << httpRequest.isValid() << "\n";
    debug << "-----------------------------------" << "\n";
    debug << httpRequest.rawHeader() << "\n";
    debug << "-----------------------------------" << "\n";
    debug << httpRequest.payload() << "\n";
    debug << "-----------------------------------" << "\n";
    return debug;
}

}
