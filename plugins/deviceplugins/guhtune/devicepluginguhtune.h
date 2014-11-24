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

#ifndef DEVICEPLUGINELRO_H
#define DEVICEPLUGINELRO_H

#include "plugin/deviceplugin.h"

#include <QDebug>
#include <QTimer>

class GuhButton;
class GuhEncoder;
class GuhTouch;
class GuhTuneUiServer;

class DevicePluginGuhTune : public DevicePlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "guru.guh.DevicePlugin" FILE "devicepluginguhtune.json")
    Q_INTERFACES(DevicePlugin)

public:
    explicit DevicePluginGuhTune();

    DeviceManager::HardwareResources requiredHardware() const override;
    void startMonitoringAutoDevices() override;
    DeviceManager::DeviceSetupStatus setupDevice(Device *device) override;

private:
    GuhButton *m_button;
    GuhEncoder *m_encoder;
    GuhTouch *m_touch;

    GuhTuneUiServer *m_uiServer;

private slots:
    void buttonPressed();
    void buttonReleased();
    void buttonLongPressed();
    void handDetected();
    void handDisappeared();
    void encoderIncreased();
    void encoderDecreased();
    void navigationLeft();
    void navigationRight();
};

#endif // DEVICEPLUGINELRO_H
