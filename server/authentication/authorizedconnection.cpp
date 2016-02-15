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
    \class guhserver::AuthorizedConnection
    \brief Represents an authorized connection to the API.

    \inmodule core


*/

#include "authorizedconnection.h"

namespace guhserver {

/*! Constructs an AuthorizedConnection. */
AuthorizedConnection::AuthorizedConnection()
{

}

/*! Constructs a copy of the given \a other AuthorizedConnection. */
AuthorizedConnection::AuthorizedConnection(const AuthorizedConnection &other)
{
    m_userId = other.userId();
    m_token = other.token();
    m_clientDescription = other.clientDescription();
    m_lastLogin = other.lastLogin();
}

/*! Returns the \l{UserId} of the \l{User} which created this \l{AuthorizedConnection}. */
UserId AuthorizedConnection::userId() const
{
    return m_userId;
}

/*! Sets the \l{UserId} of the \l{User} which created this \l{AuthorizedConnection} to the given \a userId. */
void AuthorizedConnection::setUserId(const UserId &userId)
{
    m_userId = userId;
}

/*! Returns the token of this \l{AuthorizedConnection}. */
QString AuthorizedConnection::token() const
{
    return m_token;
}

/*! Sets the token of this \l{AuthorizedConnection} to the given \a token. */
void AuthorizedConnection::setToken(const QString &token)
{
    m_token = token;
}

/*! Returns the client description of this \l{AuthorizedConnection}. */
QString AuthorizedConnection::clientDescription() const
{
    return m_clientDescription;
}

/*! Sets the client description of this \l{AuthorizedConnection} to the given \a clientDescription. */
void AuthorizedConnection::setClientDescription(const QString &clientDescription)
{
    m_clientDescription = clientDescription;
}

/*! Returns the unix timestamp when this \l{AuthorizedConnection} was used the last time. */
quint64 AuthorizedConnection::lastLogin() const
{
    return m_lastLogin;
}

/*! Sets the unix timestamp to \a lastLogin when this \l{AuthorizedConnection} was used the last time. */
void AuthorizedConnection::setLastLogin(const quint64 &lastLogin)
{
    m_lastLogin = lastLogin;
}

}
