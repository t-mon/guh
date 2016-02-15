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
    \class guhserver::User
    \brief Represents an authorized user of the server.

    \inmodule core

*/

#include "user.h"

namespace guhserver {

/*! Constructs an \l{User}. */
User::User() :
    m_isAdmin(false)
{
}

/*! Constructs a copy of the given \a other \l{User}. */
User::User(const User &other)
{
    m_userId = other.userId();
    m_userName = other.userName();
    m_password = other.password();
    m_isAdmin = other.isAdmin();
}

/*! Returns the \l{UserId} of this \l{User}. */
UserId User::userId() const
{
    return m_userId;
}

/*! Returns the \l{UserId} of this \l{User} to the given \a userId. */
void User::setUserId(const UserId &userId)
{
    m_userId = userId;
}

/*! Returns the user name of this \l{User}. */
QString User::userName() const
{
    return m_userName;
}

/*! Sets the user name of this \l{User} to the given \a userName. */
void User::setUserName(const QString &userName)
{
    m_userName = userName;
}

/*! Returns the hashed password of this \l{User}. */
QString User::password() const
{
    return m_password;
}

/*! Sets the password of this \l{User} to the given hashed \a password. */
void User::setPassword(const QString &password)
{
    m_password = password;
}

/*! Returns true if this \l{User} is an administrator. */
bool User::isAdmin() const
{
    return m_isAdmin;
}

/*! Sets the administrator permissions of this \l{User} to the given \a isAdmin parameter. */
void User::setAdmin(const bool &isAdmin)
{
    m_isAdmin = isAdmin;
}

/*! Returns true if this \l{User} has a valid \l{UserId}, user name and password. */
bool User::isValid()
{
    return !m_userId.isNull() && !m_userName.isEmpty() && !m_password.isEmpty();
}

}
