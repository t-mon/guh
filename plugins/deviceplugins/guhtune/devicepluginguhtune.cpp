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
#include <QByteArray>

DeviceClassId guhTuneItemDeviceClassId = DeviceClassId("4a20b247-576c-4a0f-bc95-9eb6b2f2eb3d");
ActionTypeId toggleActionTypeId = ActionTypeId("79780b65-692c-4431-aede-a9dcfc10dce6");
ActionTypeId setValueActionTypeId = ActionTypeId("e664dc4f-d0d0-4eec-b23d-1e2b711b50ee");
StateTypeId powerStateTypeId = StateTypeId("97508d06-9c48-464f-8132-e4a923ed55f5");
StateTypeId valueStateTypeId = StateTypeId("db22405e-1217-4e25-a94e-334f123cb40c");

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

    DeviceDescriptor deviceDescriptorCouch(guhTuneItemDeviceClassId, "guhTune Couch");
    ParamList paramsCouch;
    paramsCouch.append(Param("item", 1));
    deviceDescriptorCouch.setParams(paramsCouch);
    deviceDescriptorList.append(deviceDescriptorCouch);

    DeviceDescriptor deviceDescriptorWork(guhTuneItemDeviceClassId, "guhTune Working");
    ParamList paramsWork;
    paramsWork.append(Param("item", 2));
    deviceDescriptorWork.setParams(paramsWork);
    deviceDescriptorList.append(deviceDescriptorWork);

    DeviceDescriptor deviceDescriptorSwitch(guhTuneItemDeviceClassId, "guhTune Switch");
    ParamList paramsSwitch;
    paramsSwitch.append(Param("item", 3));
    deviceDescriptorSwitch.setParams(paramsSwitch);
    deviceDescriptorList.append(deviceDescriptorSwitch);

    DeviceDescriptor deviceDescriptorRadiator(guhTuneItemDeviceClassId, "guhTune Radiator");
    ParamList paramsRadiator;
    paramsRadiator.append(Param("item", 4));
    deviceDescriptorRadiator.setParams(paramsRadiator);
    deviceDescriptorList.append(deviceDescriptorRadiator);

    emit autoDevicesAppeared(guhTuneItemDeviceClassId, deviceDescriptorList);
}

DeviceManager::DeviceSetupStatus DevicePluginGuhTune::setupDevice(Device *device)
{
    // initialize the hardware only once
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
            m_encoder->setNavigationSensitivity(10);

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

        // TCP Server for guhTuneUi
        m_uiServer = new GuhTuneUiServer(this);
        if (m_uiServer->startServer()) {
            qDebug() << " ----> guhTune UI server running";
            connect(m_uiServer, SIGNAL(dataAvailable(QByteArray)), this, SLOT(dataAvailable(QByteArray)));

        } else {
            m_uiServer->deleteLater();
            qDebug() << " ----> ERROR: guhTune UI server could not be initialized.";
        }
    }

    if(device->paramValue("item").toInt() == 1) {
        device->setName("guhTune | Couch (" + device->paramValue("item").toString() + ")");
    } else if (device->paramValue("item").toInt() == 2) {
        device->setName("guhTune | Work (" + device->paramValue("item").toString() + ")");
    } else if (device->paramValue("item").toInt() == 3) {
        device->setName("guhTune | Switch (" + device->paramValue("item").toString() + ")");
    } else if (device->paramValue("item").toInt() == 4) {
        device->setName("guhTune | Radiator (" + device->paramValue("item").toString() + ")");
    } else {
        return DeviceManager::DeviceSetupStatusFailure;
    }

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

void DevicePluginGuhTune::dataAvailable(const QByteArray &data)
{
    int item = data.left(1).toInt();
    QByteArray message = data.right(data.length() - 2);

    qDebug() << "item = " << item << " | message = " << message;

    Device *device;

    // get the device with the item id
    foreach (Device* configuredDevice, myDevices()) {
        if (configuredDevice->paramValue("item").toInt() == item){
            device = configuredDevice;
            break;
        }
    }

    if (device) {
        if (message == "toggle") {
            bool currentState = device->stateValue(powerStateTypeId).toBool();
            device->setStateValue(powerStateTypeId, !currentState);
            qDebug() << "Toggle device " << item << " to " << !currentState;
            return;
        } else {
            bool ok;
            int value = message.toInt(&ok);
            if(ok){
                device->setStateValue(valueStateTypeId, value);
                qDebug() << "Set item " << item << " value to " << value;
            }

            // set the brightness of the ambilight
//            foreach (Device* ambilight, deviceManager()->findConfiguredDevices(DeviceClassId("1647c61c-db14-461e-8060-8a3533d5d92f"))) {
//                qDebug() << "Found Boblight device -> " << ambilight->name() << " ...and set brightness to " << value;

//            }
            return;
        }
    }
}
