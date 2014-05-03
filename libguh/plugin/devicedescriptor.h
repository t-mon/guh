/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
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

#ifndef DEVICEDESCRIPTION_H
#define DEVICEDESCRIPTION_H

#include <typeutils.h>
#include <types/param.h>

#include <QVariantMap>

class DeviceDescriptor
{
public:
    DeviceDescriptor();
    DeviceDescriptor(const DeviceClassId &deviceClassId, const QString &title = QString(), const QString &description = QString());
    DeviceDescriptor(const DeviceDescriptorId &id, const DeviceClassId &deviceClassId, const QString &title = QString(), const QString &description = QString());

    bool isValid() const;

    DeviceDescriptorId id() const;
    DeviceClassId deviceClassId() const;

    QString title() const;
    void setTitle(const QString &title);

    QString description() const;
    void setDescription(const QString &description);

    QList<Param> params() const;
    void setParams(const QList<Param> &params);

private:
    DeviceDescriptorId m_id;
    DeviceClassId m_deviceClassId;
    QString m_title;
    QString m_description;
    QList<Param> m_params;
};

#endif // DEVICEDESCRIPTION_H