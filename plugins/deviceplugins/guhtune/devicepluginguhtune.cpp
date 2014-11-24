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

#include "devicepluginguhtune.h"
#include "devicemanager.h"
#include "guhbutton.h"
#include "guhencoder.h"
#include "guhtouch.h"
#include "guhtuneuiserver.h"

#include <QDebug>
#include <QStringList>

DeviceClassId guhTuneItemDeviceClassId = DeviceClassId("4a20b247-576c-4a0f-bc95-9eb6b2f2eb3d");
EventTypeId selectedEventTypeId = EventTypeId("eeed2786-02eb-4a57-8e61-149741afb58c");
EventTypeId setValueEventTypeId = EventTypeId("1bc20ff4-4a53-4346-9477-c2c15fc11d61");

DevicePluginGuhTune::DevicePluginGuhTune()
{
}

void DevicePluginGuhTune::startMonitoringAutoDevices()
{
    qDebug() << "*****************************************";
    // make shore, we add only one guhTune device
    foreach (Device *device, myDevices()) {
        if (device->deviceClassId() == guhTuneItemDeviceClassId) {
            return;
        }
    }

    QList<DeviceDescriptor> deviceDescriptorList;
    DeviceDescriptor deviceDescriptor(guhTuneItemDeviceClassId, "guhTune device");
    deviceDescriptorList.append(deviceDescriptor);
    emit autoDevicesAppeared(guhTuneItemDeviceClassId, deviceDescriptorList);
}

DeviceManager::DeviceSetupStatus DevicePluginGuhTune::setupDevice(Device *device)
{
    if (myDevices().isEmpty()) {
        // Hardware pushbutton
        m_button = new GuhButton(this, 44);
        if(m_button->enable()){
            qDebug() << " ----> hardware button found.";
            connect(m_button, &GuhButton::buttonPressed, this, &DevicePluginGuhTune::buttonPressed);
            connect(m_button, &GuhButton::buttonReleased, this, &DevicePluginGuhTune::buttonReleased);
            connect(m_button, &GuhButton::buttonLongPressed, this, &DevicePluginGuhTune::buttonLongPressed);
        } else {
            qDebug() << " ----> ERROR: hardware button NOT found.";
            m_button->deleteLater();
        }

        // Hardware encoder
        m_encoder = new GuhEncoder(this, 47, 46, 65);
        if(m_encoder->enable()){
            qDebug() << " ----> hardware encoder found.";

            m_encoder->setBasicSensitivity(2);
            m_encoder->setNavigationSensitivity(4);

            connect(m_encoder, &GuhEncoder::increased, this, &DevicePluginGuhTune::encoderIncreased);
            connect(m_encoder, &GuhEncoder::decreased, this, &DevicePluginGuhTune::encoderDecreased);
            connect(m_encoder, &GuhEncoder::navigationLeft, this, &DevicePluginGuhTune::navigationLeft);
            connect(m_encoder, &GuhEncoder::navigationRight, this, &DevicePluginGuhTune::navigationRight);
            connect(m_encoder, &GuhEncoder::buttonPressed, this, &DevicePluginGuhTune::buttonPressed);
            connect(m_encoder, &GuhEncoder::buttonReleased, this, &DevicePluginGuhTune::buttonReleased);
        } else {
            qDebug() << " ----> ERROR: hardware button NOT found.";
            m_encoder->deleteLater();
        }

        // Hardware touch sensor
        m_touch = new GuhTouch(this, 26);
        if(m_touch->enable()){
            qDebug() << " ----> hardware touch sensor found.";
            connect(m_touch, &GuhTouch::handDetected, this, &DevicePluginGuhTune::handDetected);
            connect(m_touch, &GuhTouch::handDisappeared, this, &DevicePluginGuhTune::handDisappeared);
        } else {
            qDebug() << " ----> ERROR: hardware touch sensor NOT found.";
            m_touch->deleteLater();
        }

        m_uiServer = new GuhTuneUiServer(this);
        if (m_uiServer->startServer()) {
            qDebug() << " ----> guhTune UI server running";
        } else {
            m_uiServer->deleteLater();
            qDebug() << " ----> ERROR: guhTune UI server could not be initialized.";
        }

    }

    device->setName("guhTune item (" + device->paramValue("item").toString() + ")");

    Q_UNUSED(device);
    return DeviceManager::DeviceSetupStatusSuccess;
}

DeviceManager::HardwareResources DevicePluginGuhTune::requiredHardware() const
{
    return DeviceManager::HardwareResourceNone;
}

void DevicePluginGuhTune::buttonPressed()
{
    qDebug() << "button pressed";
    if (m_uiServer) {
        m_uiServer->buttonPressed();
    }
}

void DevicePluginGuhTune::buttonReleased()
{
    qDebug() << "button released";
    if (m_uiServer) {
        m_uiServer->buttonReleased();
    }
}

void DevicePluginGuhTune::buttonLongPressed()
{
    qDebug() << "button long pressed";
    if (m_uiServer) {
        m_uiServer->buttonLongPressed();
    }
}

void DevicePluginGuhTune::handDetected()
{
    qDebug() << "hand detected";
    if (m_uiServer) {
        m_uiServer->handDetected();
    }
}

void DevicePluginGuhTune::handDisappeared()
{
    qDebug() << "hand disappeared";
    if (m_uiServer) {
        m_uiServer->handDisappeard();
    }
}

void DevicePluginGuhTune::encoderIncreased()
{
    qDebug() << "encoder +";
    if (m_uiServer) {
        m_uiServer->tickRight();
    }
}

void DevicePluginGuhTune::encoderDecreased()
{
    qDebug() << "encoder -";
    if (m_uiServer) {
        m_uiServer->tickLeft();
    }
}

void DevicePluginGuhTune::navigationLeft()
{
    qDebug() << "navigation LEFT     <--- ";
    if (m_uiServer) {
        m_uiServer->navigateLeft();
    }
}

void DevicePluginGuhTune::navigationRight()
{
    qDebug() << "navigation Right    ---> ";
    if (m_uiServer) {
        m_uiServer->navigateRight();
    }
}
