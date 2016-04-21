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

#ifndef DEVICEPLUGINSENSORTAG_H
#define DEVICEPLUGINSENSORTAG_H

#ifdef BLUETOOTH_LE

#include "plugin/deviceplugin.h"
#include "bluetooth/bluetoothlowenergydevice.h"
#include "sensortag.h"

class DevicePluginSensorTag : public DevicePlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "guru.guh.DevicePlugin" FILE "devicepluginsensortag.json")
    Q_INTERFACES(DevicePlugin)

public:
    explicit DevicePluginSensorTag();

    DeviceManager::DeviceError discoverDevices(const DeviceClassId &deviceClassId, const ParamList &params) override;
    DeviceManager::DeviceSetupStatus setupDevice(Device *device) override;
    DeviceManager::HardwareResources requiredHardware() const override;
    DeviceManager::DeviceError executeAction(Device *device, const Action &action) override;

    void bluetoothDiscoveryFinished(const QList<QBluetoothDeviceInfo> &deviceInfos);
    void deviceRemoved(Device *device) override;

    void guhTimer() override;

private:
    QHash<SensorTag *, Device *> m_sensors;

private slots:
    void onConnectionChanged();

};
#endif // BLUETOOTH_LE

#endif // DEVICEPLUGINSENSORTAG_H
