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

#ifndef AUTHENTICATEDCONNECTION_H
#define AUTHENTICATEDCONNECTION_H

#include <QObject>

#include "user.h"

namespace guhserver {

class AuthorizedConnection
{
public:
    explicit AuthorizedConnection();

    User user() const;
    void setUser(const User &user);

    QString token() const;
    void setToken(const QString &token);

    QString clientDescription() const;
    void setClientDescription(const QString &clientDescription);

    quint64 lastLogin() const;
    void setLastLogin(const quint64 &lastLogin);

private:
    User m_user;
    QString m_token;
    QString m_clientDescription;
    quint64 m_lastLogin;

};

}

#endif // AUTHENTICATEDCONNECTION_H
