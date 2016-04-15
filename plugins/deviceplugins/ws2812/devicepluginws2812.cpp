/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2015 Simon Stuerz <simon.stuerz@guh.guru>                *
 *  Copyright (C) 2016 Bernhard Trinnes <bernhard.trinnes@guh.guru>        *
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
    \page ws2812.html
    \title ws2812
    \brief Plugin to controll ws2812 LEDs based on 6LoWPAN networking.

    \ingroup plugins
    \ingroup guh-plugins-merkur

    The plugin for the \l{http://cdn.sparkfun.com/datasheets/Components/LED/WS2812.pdf}{ws2812} led module. This plugin allowes to control a ws2812 module over a 6LoWPAN network.

    \chapter Plugin properties
    Following JSON file contains the definition and the description of all available \l{DeviceClass}{DeviceClasses}
    and \l{Vendor}{Vendors} of this \l{DevicePlugin}.

    For more details how to read this JSON file please check out the documentation for \l{The plugin JSON File}.

    \quotefile plugins/deviceplugins/ws2812/devicepluginws2812.json
*/

#include "devicepluginws2812.h"
#include "plugin/device.h"
#include "plugininfo.h"


#include <QDebug>
#include <QStringList>
#include <QColor>

DevicePluginWs2812::DevicePluginWs2812()
{

}

DeviceManager::HardwareResources DevicePluginWs2812::requiredHardware() const
{
    // We need the NetworkManager for node discovery and the timer for ping requests
    return DeviceManager::HardwareResourceNetworkManager | DeviceManager::HardwareResourceTimer;
}

DeviceManager::DeviceSetupStatus DevicePluginWs2812::setupDevice(Device *device)
{
    qCDebug(dcWs2812) << "Setup Plant Care" << device->name() << device->params();

    // Check if device already added with this address
    if (deviceAlreadyAdded(QHostAddress(device->paramValue("host").toString()))) {
        qCWarning(dcWs2812) << "Device with this address already added.";
        return DeviceManager::DeviceSetupStatusFailure;
    }

    // Create the CoAP socket if not already created
    if (m_coap.isNull()) {
        m_coap = new CoapNetworkAccessManager(this);
        connect(m_coap.data(), SIGNAL(replyFinished(CoapReply*)), this, SLOT(coapReplyFinished(CoapReply*)));
        connect(m_coap.data(), SIGNAL(notificationReceived(CoapObserveResource,int,QByteArray)), this, SLOT(onNotificationReceived(CoapObserveResource,int,QByteArray)));
    }

    return DeviceManager::DeviceSetupStatusSuccess;
}

void DevicePluginWs2812::deviceRemoved(Device *device)
{
    Q_UNUSED(device)

    // Delete the CoAP socket if there are no devices left
    if (myDevices().isEmpty()) {
        m_coap->deleteLater();
    }
}

DeviceManager::DeviceError DevicePluginWs2812::discoverDevices(const DeviceClassId &deviceClassId, const ParamList &params)
{
    Q_UNUSED(params)

    // Perform a HTTP GET on the RPL router address
    QHostAddress address(configuration().paramValue("RPL address").toString());
    qCDebug(dcWs2812) << "Scan for new nodes on RPL" << address.toString();

    QUrl url;
    url.setScheme("http");
    url.setHost(address.toString());

    m_asyncNodeScans.insert(networkManagerGet(QNetworkRequest(url)), deviceClassId);
    return DeviceManager::DeviceErrorAsync;
}

void DevicePluginWs2812::networkManagerReplyReady(QNetworkReply *reply)
{
    if (m_asyncNodeScans.keys().contains(reply)) {
        DeviceClassId deviceClassId = m_asyncNodeScans.take(reply);
        // Check HTTP status code
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
            qCWarning(dcWs2812) << "Node scan reply HTTP error:" << reply->errorString();
            emit devicesDiscovered(deviceClassId, QList<DeviceDescriptor>());
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        qCDebug(dcWs2812) << "Node discovery finished:" << endl << data;

        QList<DeviceDescriptor> deviceDescriptors;
        QList<QByteArray> lines = data.split('\n');
        qCDebug(dcWs2812) << lines;
        foreach (const QByteArray &line, lines) {
            if (line.isEmpty())
                continue;

            QHostAddress address(QString(line.left(line.length() - 4)));
            if (address.isNull())
                continue;

            qCDebug(dcWs2812) << "Found node" << address.toString();
            // Create a deviceDescriptor for each found address
            DeviceDescriptor descriptor(deviceClassId, "ws2812", address.toString());
            ParamList params;
            params.append(Param("host", address.toString()));
            descriptor.setParams(params);
            deviceDescriptors.append(descriptor);
        }
        // Inform the user which devices were found
        emit devicesDiscovered(deviceClassId, deviceDescriptors);
    }

    // Delete the HTTP reply
    reply->deleteLater();
}

void DevicePluginWs2812::postSetupDevice(Device *device)
{
    // Try to ping the device after a successful setup
    pingDevice(device);
}

void DevicePluginWs2812::guhTimer()
{
    // Try to ping each device every 10 seconds to make sure it is still reachable
    foreach (Device *device, myDevices()) {
        if (device->deviceClassId() == ws2812DeviceClassId) {
            pingDevice(device);
        }
    }
}

DeviceManager::DeviceError DevicePluginWs2812::executeAction(Device *device, const Action &action)
{
    if (device->deviceClassId() != ws2812DeviceClassId)
        return DeviceManager::DeviceErrorDeviceClassNotFound;

    qCDebug(dcWs2812) << "Execute action" << device->name() << action.params();

    // Check if the device is reachable
    if (!device->stateValue(reachableStateTypeId).toBool()) {
        qCWarning(dcWs2812) << "Device not reachable.";
        return DeviceManager::DeviceErrorHardwareNotAvailable;
    }

    if(action.actionTypeId() == effectColorActionTypeId) {

        QUrl url;
        url.setScheme("coap");
        url.setHost(device->paramValue("host").toString());
        url.setPath("/a/color");

        QColor newColor = action.param("color").value().value<QColor>().toRgb();
        QByteArray message = "color=" + newColor.name().remove("#").toUtf8();

        qCDebug(dcWs2812) << "Sending" << url.toString() << message;

        CoapReply *reply = m_coap->post(CoapRequest(url), message);
        if (reply->isFinished() && reply->error() != CoapReply::NoError) {
            qCWarning(dcWs2812) << "CoAP reply finished with error" << reply->errorString();
            setReachable(device, false);
            reply->deleteLater();
            return DeviceManager::DeviceErrorHardwareFailure;
        }
        m_setColor.insert(reply, action);
        m_asyncActions.insert(action.id(), device);
        return DeviceManager::DeviceErrorAsync;


    } else if(action.actionTypeId() == maxPixActionTypeId) {

        QUrl url;
        url.setScheme("coap");
        url.setHost(device->paramValue("host").toString());
        url.setPath("/p/maxpix");

        //QColor color = action.param("color").value().value<QColor>().toHsv();
        //QColor newColor = QColor::fromHsv(color.hue(), color.saturation(), 100 * light->brightness() / 255.0);
        //QByteArray message = "color=" + newColor.toRgb().name().remove("#").toUtf8();

        int max = action.param("leds").value().toInt();

        qCDebug(dcWs2812) << "Max Pix" << max;

        QByteArray message = QString("max=%1").arg(QString::number(max)).toUtf8();

        //QByteArray message = "max=" + action.param("maxPix").value().toByteArray();

        qCDebug(dcWs2812) << "Sending" << url.toString() << message;

        CoapReply *reply = m_coap->post(CoapRequest(url), message);
        if (reply->isFinished() && reply->error() != CoapReply::NoError) {
            qCWarning(dcWs2812) << "CoAP reply finished with error" << reply->errorString();
            setReachable(device, false);
            reply->deleteLater();
            return DeviceManager::DeviceErrorHardwareFailure;
        }
        m_setPix.insert(reply, action);
        m_asyncActions.insert(action.id(), device);
        return DeviceManager::DeviceErrorAsync;


    } else if(action.actionTypeId() == effectModeActionTypeId) {

        //int effectmode = createSetColorRequest(action.param("color").value().value<QColor>());

        int effectmode = 0;

        QString effectModeString = action.param("effect").value().toString();


        QUrl url;
        url.setScheme("coap");
        url.setHost(device->paramValue("host").toString());
        url.setPath("/a/effect");


        /*"Off",
        "Color On",
        "Color Wave",
        "Color Fade",
        "Color Flash",
        "Rainbow Wave",
        "Rainbow Flash",
        "Knight Rider",
        "Fire"
        */

        qCDebug(dcWs2812) << "Set effect mode to:" << effectModeString;

        if(effectModeString == "Off") {
            effectmode = 0;
        } else if (effectModeString == "Color On") {
            effectmode = 1;
        } else if (effectModeString == "Color Wave") {
            effectmode = 2;
        } else if (effectModeString == "Color Fade") {
            effectmode = 3;
        }else if (effectModeString == "Color Flash") {
            effectmode = 4;
        }else if (effectModeString == "Rainbow Wave") {
            effectmode = 5;
        }else if (effectModeString == "Rainbow Flash") {
            effectmode = 6;
        }else if (effectModeString == "Knight Rider") {
            effectmode = 7;
        }else if (effectModeString == "Fire") {
            effectmode = 8;
        }

        QByteArray payload = QString("mode=%1").arg(QString::number(effectmode)).toUtf8();
        qCDebug(dcWs2812()) << "Sending" << payload << url.path();

        CoapReply *reply = m_coap->post(CoapRequest(url), payload);
        if (reply->isFinished() && reply->error() != CoapReply::NoError) {
            qCWarning(dcWs2812) << "CoAP reply finished with error" << reply->errorString();
            setReachable(device, false);
            reply->deleteLater();
            return DeviceManager::DeviceErrorHardwareFailure;
        }

        m_setEffect.insert(reply, action);
        m_asyncActions.insert(action.id(), device);
        return DeviceManager::DeviceErrorAsync;

    }




    return DeviceManager::DeviceErrorActionTypeNotFound;
}

void DevicePluginWs2812::pingDevice(Device *device)
{
    QUrl url;
    url.setScheme("coap");
    url.setHost(device->paramValue("host").toString());
    m_pingReplies.insert(m_coap->ping(CoapRequest(url)), device);
}

void DevicePluginWs2812::updateBattery(Device *device)
{
    qCDebug(dcWs2812) << "Update" << device->name() << "battery value";
    QUrl url;
    url.setScheme("coap");
    url.setHost(device->paramValue("host").toString());
    url.setPath("/s/battery");
    CoapReply *reply = m_coap->get(CoapRequest(url));
    if (reply->isFinished() && reply->error() != CoapReply::NoError) {
        qCWarning(dcWs2812) << "CoAP reply finished with error" << reply->errorString();
        setReachable(device, false);
        reply->deleteLater();
        return;
    }
    m_updateReplies.insert(reply, device);
}



void DevicePluginWs2812::updateColor(Device *device)
{
    qCDebug(dcWs2812) << "Update" << device->name() << "color value";
    QUrl url;
    url.setScheme("coap");
    url.setHost(device->paramValue("host").toString());
    url.setPath("/a/color");
    CoapReply *reply = m_coap->get(CoapRequest(url));
    if (reply->isFinished() && reply->error() != CoapReply::NoError) {
        qCWarning(dcWs2812) << "CoAP reply finished with error" << reply->errorString();
        setReachable(device, false);
        reply->deleteLater();
        return;
    }

    m_updateReplies.insert(reply, device);
}



void DevicePluginWs2812::updateEffect(Device *device)
{
    qCDebug(dcWs2812) << "Update" << device->name() << "effect mode";
    QUrl url;
    url.setScheme("coap");
    url.setHost(device->paramValue("host").toString());
    url.setPath("/a/effect");
    CoapReply *reply = m_coap->get(CoapRequest(url));
    if (reply->isFinished() && reply->error() != CoapReply::NoError) {
        qCWarning(dcWs2812) << "CoAP reply finished with error" << reply->errorString();
        setReachable(device, false);
        reply->deleteLater();
        return;
    }
    m_updateReplies.insert(reply, device);
}


void DevicePluginWs2812::updateMaxPix(Device *device)
{
    qCDebug(dcWs2812) << "Update" << device->name() << "max pix";
    QUrl url;
    url.setScheme("coap");
    url.setHost(device->paramValue("host").toString());
    url.setPath("/p/maxpix");
    CoapReply *reply = m_coap->get(CoapRequest(url));
    if (reply->isFinished() && reply->error() != CoapReply::NoError) {
        qCWarning(dcWs2812) << "CoAP reply finished with error" << reply->errorString();
        setReachable(device, false);
        reply->deleteLater();
        return;
    }
    m_updateReplies.insert(reply, device);
}



void DevicePluginWs2812::enableNotifications(Device *device)
{
    qCDebug(dcWs2812) << "Enable" << device->name() << "notifications";
    QUrl url;
    url.setScheme("coap");
    url.setHost(device->paramValue("host").toString());


    url.setPath("/s/battery");
    m_enableNotification.insert(m_coap->enableResourceNotifications(CoapRequest(url)), device);

    url.setPath("/p/maxpix");
    m_enableNotification.insert(m_coap->enableResourceNotifications(CoapRequest(url)), device);

    url.setPath("/a/color");
    m_enableNotification.insert(m_coap->enableResourceNotifications(CoapRequest(url)), device);

    url.setPath("/a/effect");
    m_enableNotification.insert(m_coap->enableResourceNotifications(CoapRequest(url)), device);

}

void DevicePluginWs2812::setReachable(Device *device, const bool &reachable)
{
    if (device->stateValue(reachableStateTypeId).toBool() != reachable) {
        if (!reachable) {
            // Warn just once that the device is not reachable
            qCWarning(dcWs2812()) << device->name() << "reachable changed" << reachable;
        } else {
            qCDebug(dcWs2812()) << device->name() << "reachable changed" << reachable;

            // Get current state values after a reconnect
            updateBattery(device);
            updateColor(device);
            updateEffect(device);
            updateMaxPix(device);

            // Make sure the notifications are enabled
            enableNotifications(device);
        }
    }

    device->setStateValue(reachableStateTypeId, reachable);
}

bool DevicePluginWs2812::deviceAlreadyAdded(const QHostAddress &address)
{
    // Check if we already have a device with the given address
    foreach (Device *device, myDevices()) {
        if (device->paramValue("host").toString() == address.toString()) {
            return true;
        }
    }
    return false;
}

Device *DevicePluginWs2812::findDevice(const QHostAddress &address)
{
    // Return the device pointer with the given address (otherwise 0)
    foreach (Device *device, myDevices()) {
        if (device->paramValue("host").toString() == address.toString()) {
            return device;
        }
    }
    return NULL;
}

void DevicePluginWs2812::coapReplyFinished(CoapReply *reply)
{
    if (m_pingReplies.contains(reply)) {
        Device *device = m_pingReplies.take(reply);

        // Check CoAP reply error
        if (reply->error() != CoapReply::NoError) {
            if (device->stateValue(reachableStateTypeId).toBool())
                qCWarning(dcWs2812) << "Ping device" << reply->request().url().toString() << "reply finished with error" << reply->errorString();

            setReachable(device, false);
            reply->deleteLater();
            return;
        }
        setReachable(device, true);

    } else if (m_updateReplies.contains(reply)) {
        Device *device = m_updateReplies.take(reply);
        QString urlPath = reply->request().url().path();

        // Check CoAP reply error
        if (reply->error() != CoapReply::NoError) {
            qCWarning(dcWs2812) << "Update resource" << urlPath << "reply finished with error" << reply->errorString();
            setReachable(device, false);
            reply->deleteLater();
            return;
        }

        // Check CoAP status code
        if (reply->statusCode() != CoapPdu::Content) {
            qCWarning(dcWs2812) << "Update resource" << urlPath << "status code error:" << reply;
            reply->deleteLater();
            return;
        }

        // Update corresponding device state
        if (urlPath == "/s/battery") {
            qCDebug(dcWs2812()) << "Updated battery value:" << reply->payload();
            device->setStateValue(batteryStateTypeId, reply->payload().toDouble());
        } else if (urlPath == "/a/color") {
            qCDebug(dcWs2812()) << "Updated color value:" << reply->payload();
            device->setStateValue(effectColorStateTypeId, reply->payload());
        } else if (urlPath == "/p/maxpix") {
            qCDebug(dcWs2812()) << "Updated max pix value:" << reply->payload();
            device->setStateValue(maxPixStateTypeId, reply->payload());
        }

    } else if (m_setEffect.contains(reply)) {
        Action action = m_setEffect.take(reply);
        Device *device = m_asyncActions.take(action.id());

        // Check CoAP reply error
        if (reply->error() != CoapReply::NoError) {
            qCWarning(dcWs2812) << "CoAP reply  toggle light finished with error" << reply->errorString();
            setReachable(device, false);
            reply->deleteLater();
            emit actionExecutionFinished(action.id(), DeviceManager::DeviceErrorHardwareFailure);
            return;
        }

        // Check CoAP status code
        if (reply->statusCode() != CoapPdu::Content) {
            qCWarning(dcWs2812) << "Set effect code error:" << reply;
            reply->deleteLater();
            emit actionExecutionFinished(action.id(), DeviceManager::DeviceErrorHardwareFailure);
            return;
        }
        // Tell the user about the action execution result
        emit actionExecutionFinished(action.id(), DeviceManager::DeviceErrorNoError);

    } else if (m_setColor.contains(reply)) {
        Action action = m_setColor.take(reply);
        Device *device = m_asyncActions.take(action.id());

        // check CoAP reply error
        if (reply->error() != CoapReply::NoError) {
            qCWarning(dcWs2812) << "CoAP set color reply finished with error" << reply->errorString();
            setReachable(device, false);
            reply->deleteLater();
            emit actionExecutionFinished(action.id(), DeviceManager::DeviceErrorHardwareFailure);
            return;
        }

        // Check CoAP status code
        if (reply->statusCode() != CoapPdu::Content) {
            qCWarning(dcWs2812) << "Set color status code error:" << reply;
            reply->deleteLater();
            emit actionExecutionFinished(action.id(), DeviceManager::DeviceErrorHardwareFailure);
            return;
        }

        // Update the state here, so we don't have to wait for the notification
        device->setStateValue(effectColorStateTypeId, action.param("color").value().toInt());
        // Tell the user about the action execution result
        emit actionExecutionFinished(action.id(), DeviceManager::DeviceErrorNoError);

    }  else if (m_setPix.contains(reply)) {
        Action action = m_setPix.take(reply);
        Device *device = m_asyncActions.take(action.id());

        // check CoAP reply error
        if (reply->error() != CoapReply::NoError) {
            qCWarning(dcWs2812) << "CoAP set maxpix reply finished with error" << reply->errorString();
            setReachable(device, false);
            reply->deleteLater();
            emit actionExecutionFinished(action.id(), DeviceManager::DeviceErrorHardwareFailure);
            return;
        }

        // Check CoAP status code
        if (reply->statusCode() != CoapPdu::Content) {
            qCWarning(dcWs2812) << "Set max pix code error:" << reply;
            reply->deleteLater();
            emit actionExecutionFinished(action.id(), DeviceManager::DeviceErrorHardwareFailure);
            return;
        }

        // Update the state here, so we don't have to wait for the notification
        device->setStateValue(maxPixStateTypeId, action.param("maxPix").value().toInt());
        // Tell the user about the action execution result
        emit actionExecutionFinished(action.id(), DeviceManager::DeviceErrorNoError);

    } else if (m_enableNotification.contains(reply)) {
        Device *device = m_enableNotification.take(reply);

        // check CoAP reply error
        if (reply->error() != CoapReply::NoError) {
            qCWarning(dcWs2812) << "Enable notifications for" << reply->request().url().toString() << "reply finished with error" << reply->errorString();
            setReachable(device, false);
            reply->deleteLater();
            return;
        }

        // Check CoAP status code
        if (reply->statusCode() != CoapPdu::Content) {
            qCWarning(dcWs2812) << "Enable notifications for" << reply->request().url().toString() << "reply status code error" << reply->errorString();
            reply->deleteLater();
            return;
        }

        qCDebug(dcWs2812()) << "Enabled successfully notifications for" << device->name() << reply->request().url().path();
    }

    // Delete the CoAP reply
    reply->deleteLater();
}


void DevicePluginWs2812::onNotificationReceived(const CoapObserveResource &resource, const int &notificationNumber, const QByteArray &payload)
{
    qCDebug(dcWs2812) << " --> Got notification nr." << notificationNumber << resource.url().toString() << payload;
    Device *device = findDevice(QHostAddress(resource.url().host()));
    if (!device) {
        qCWarning(dcWs2812()) << "Could not find device for this notification";
        return;
    }

    // Update the corresponding device state
    if (resource.url().path() == "/s/battery") {
        device->setStateValue(batteryStateTypeId, payload.toDouble());
    }else if (resource.url().path() == "/a/color") {
        device->setStateValue(effectColorStateTypeId, QColor(QString(payload)).toRgb());
    }else if (resource.url().path() == "/p/maxpix") {
        device->setStateValue(maxPixStateTypeId, payload.toInt());
    }
}
