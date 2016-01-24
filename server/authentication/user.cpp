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

#include "user.h"

namespace guhserver {

User::User() :
    m_isAdmin(false)
{
}

User::User(const User &other)
{
    m_userId = other.userId();
    m_userName = other.userName();
    m_password = other.password();
    m_isAdmin = other.isAdmin();
}

UserId User::userId() const
{
    return m_userId;
}

void User::setUserId(const UserId &userId)
{
    m_userId = userId;
}

QString User::userName() const
{
    return m_userName;
}

void User::setUserName(const QString &userName)
{
    m_userName = userName;
}

QString User::password() const
{
    return m_password;
}

void User::setPassword(const QString &password)
{
    m_password = password;
}

bool User::isAdmin() const
{
    return m_isAdmin;
}

void User::setAdmin(const bool &isAdmin)
{
    m_isAdmin = isAdmin;
}

bool User::isValid()
{
    return !m_userId.isNull() && !m_userName.isEmpty() && !m_password.isEmpty();
}

}
