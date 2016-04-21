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
    \page sensortag.html
    \title SensorTag
    \brief Plugin for the Texas Instruments BTLE SensorTag.

    \ingroup plugins
    \ingroup guh-plugins

    http://processors.wiki.ti.com/index.php/SensorTag_User_Guide

    \chapter Plugin properties
    Following JSON file contains the definition and the description of all available \l{DeviceClass}{DeviceClasses}
    and \l{Vendor}{Vendors} of this \l{DevicePlugin}.

    For more details how to read this JSON file please check out the documentation for \l{The plugin JSON File}.

    \quotefile plugins/deviceplugins/sensortag/devicepluginsensortag.json
*/

#ifdef BLUETOOTH_LE

#include "devicepluginsensortag.h"
#include "plugin/device.h"
#include "devicemanager.h"
#include "plugininfo.h"

DevicePluginSensorTag::DevicePluginSensorTag()
{

}

DeviceManager::DeviceError DevicePluginSensorTag::discoverDevices(const DeviceClassId &deviceClassId, const ParamList &params)
{
    Q_UNUSED(params)
    Q_UNUSED(deviceClassId)

    if (!discoverBluetooth())
        return DeviceManager::DeviceErrorHardwareNotAvailable;

    return DeviceManager::DeviceErrorAsync;
}

DeviceManager::DeviceSetupStatus DevicePluginSensorTag::setupDevice(Device *device)
{
    qDebug(dcSensorTag()) << "Setup device" << device->name();

    QBluetoothAddress address = QBluetoothAddress(device->paramValue("mac address").toString());
    QString name = device->paramValue("name").toString();
    QBluetoothDeviceInfo deviceInfo = QBluetoothDeviceInfo(address, name, 0);

    SensorTag *sensorTag = new SensorTag(deviceInfo, QLowEnergyController::PublicAddress, this);
    connect(sensorTag, &SensorTag::connectionStatusChanged, this, &DevicePluginSensorTag::onConnectionChanged);
    m_sensors.insert(sensorTag, device);

    sensorTag->connectDevice();

    return DeviceManager::DeviceSetupStatusSuccess;
}

DeviceManager::HardwareResources DevicePluginSensorTag::requiredHardware() const
{
    return DeviceManager::HardwareResourceTimer | DeviceManager::HardwareResourceBluetoothLE;
}

DeviceManager::DeviceError DevicePluginSensorTag::executeAction(Device *device, const Action &action)
{
    Q_UNUSED(device)
    Q_UNUSED(action)
    return DeviceManager::DeviceErrorDeviceClassNotFound;
}

void DevicePluginSensorTag::bluetoothDiscoveryFinished(const QList<QBluetoothDeviceInfo> &deviceInfos)
{
    QList<DeviceDescriptor> deviceDescriptors;
    foreach (QBluetoothDeviceInfo deviceInfo, deviceInfos) {
        if (deviceInfo.name().contains("SensorTag")) {
            DeviceDescriptor descriptor(sensorTagDeviceClassId, "SensorTag", deviceInfo.address().toString());
            ParamList params;
            params.append(Param("name", deviceInfo.name()));
            params.append(Param("mac address", deviceInfo.address().toString()));
            descriptor.setParams(params);
            deviceDescriptors.append(descriptor);
        }
    }

    emit devicesDiscovered(sensorTagDeviceClassId, deviceDescriptors);
}

void DevicePluginSensorTag::deviceRemoved(Device *device)
{
    SensorTag *sensorTag = m_sensors.key(device);
    m_sensors.remove(sensorTag);
    sensorTag->deleteLater();
}

void DevicePluginSensorTag::guhTimer()
{
    foreach (SensorTag *sensorTag, m_sensors.keys()) {
        if (!sensorTag->isConnected()) {
            sensorTag->connectDevice();
        }
    }
}

void DevicePluginSensorTag::onConnectionChanged()
{
    SensorTag *sensorTag = static_cast<SensorTag *>(sender());
    Device *device = m_sensors.value(sensorTag);
    device->setStateValue(connectedStateTypeId, sensorTag->isConnected());
    qCDebug(dcSensorTag()) << "Connection status changed" << sensorTag->address().toString() << sensorTag->isConnected();
}


#endif // BLUETOOTH_LE


