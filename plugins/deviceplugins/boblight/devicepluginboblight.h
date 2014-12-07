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

#ifndef DEVICEPLUGINBOBLIGHT_H
#define DEVICEPLUGINBOBLIGHT_H

#include "plugin/deviceplugin.h"

#include <QProcess>
#include "coloranimation.h"

class BobClient;

class DevicePluginBoblight : public DevicePlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "guru.guh.DevicePlugin" FILE "devicepluginboblight.json")
    Q_INTERFACES(DevicePlugin)

public:
    explicit DevicePluginBoblight();

    DeviceManager::HardwareResources requiredHardware() const override;

    void startMonitoringAutoDevices() override;
    DeviceManager::DeviceSetupStatus setupDevice(Device *device) override;

    QList<ParamType> configurationDescription() const override;

public slots:
    DeviceManager::DeviceError executeAction(Device *device, const Action &action);

private slots:
    void connectToBoblight();
    void updateColor(const int &channel, const QColor &newColor);
    void animationFinished(ActionId actionId, QColor color);

private:
    BobClient *m_bobClient;
    QHash<ColorAnimation*, Device*> m_runningAnimations;
};

#endif // DEVICEPLUGINBOBLIGHT_H
