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

#include "authorizedconnection.h"

namespace guhserver {

AuthorizedConnection::AuthorizedConnection()
{

}

AuthorizedConnection::AuthorizedConnection(const AuthorizedConnection &other)
{
    m_user = other.user();
    m_token = other.token();
    m_clientDescription = other.clientDescription();
    m_lastLogin = other.lastLogin();
}

User AuthorizedConnection::user() const
{
    return m_user;
}

void AuthorizedConnection::setUser(const User &user)
{
    m_user = user;
}

QString AuthorizedConnection::token() const
{
    return m_token;
}

void AuthorizedConnection::setToken(const QString &token)
{
    m_token = token;
}

QString AuthorizedConnection::clientDescription() const
{
    return m_clientDescription;
}

void AuthorizedConnection::setClientDescription(const QString &clientDescription)
{
    m_clientDescription = clientDescription;
}

quint64 AuthorizedConnection::lastLogin() const
{
    return m_lastLogin;
}

void AuthorizedConnection::setLastLogin(const quint64 &lastLogin)
{
    m_lastLogin = lastLogin;
}

}
