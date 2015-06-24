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

#ifndef DEVICEPLUGINTUNE_H
#define DEVICEPLUGINTUNE_H

#include "plugin/deviceplugin.h"
#include "jsonrpcserver.h"

class JsonRpcServer;

class DevicePluginTune : public DevicePlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "guru.guh.DevicePlugin" FILE "deviceplugintune.json")
    Q_INTERFACES(DevicePlugin)

public:
    explicit DevicePluginTune();

    DeviceManager::HardwareResources requiredHardware() const override;
    void startMonitoringAutoDevices() override;
    DeviceManager::DeviceSetupStatus setupDevice(Device *device) override;
    void postSetupDevice(Device *device) override;
    void deviceRemoved(Device *device) override;

private:
    JsonRpcServer *m_server;
    DeviceId m_tuneDeviceId;
    bool sync();

    bool tuneAlreadyAdded();
    void tuneAutodetected();

    void printCurrentItemList();

private slots:
    void tuneConnectionStatusChanged(const bool &connected);
    void updateMood(const QVariantMap &message);
    void updateTune(const QVariantMap &message);
    void onTodoEvent(const QVariantMap &message);
    void processActionResponse(const QVariantMap &message);

public slots:
    DeviceManager::DeviceError executeAction(Device *device, const Action &action) override;

};

#endif // DEVICEPLUGINTUNE_H
