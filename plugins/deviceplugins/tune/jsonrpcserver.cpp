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

#include "jsonrpcserver.h"
#include "loggingcategories.h"

extern PluginId pluginId;
extern DeviceClassId moodDeviceClassId;
extern DeviceClassId todoDeviceClassId;
extern StateTypeId activeStateTypeId;
extern ActionTypeId activeActionTypeId;
extern StateTypeId valueStateTypeId;
extern ActionTypeId valueActionTypeId;
extern DeviceClassId tuneDeviceClassId;
extern StateTypeId reachableStateTypeId;
extern StateTypeId approximationDetectedStateTypeId;
extern StateTypeId temperatureStateTypeId;
extern StateTypeId humidityStateTypeId;
extern StateTypeId lightIntensityStateTypeId;
extern StateTypeId powerStateTypeId;
extern ActionTypeId powerActionTypeId;
extern StateTypeId brightnessStateTypeId;
extern ActionTypeId brightnessActionTypeId;

JsonRpcServer::JsonRpcServer(QObject *parent) :
    QObject(parent),
    m_id(0)
{
    m_manager = new TuneManager(31337, this);

    connect(m_manager, &TuneManager::tuneConnectionStatusChanged, this, &JsonRpcServer::connectionStatusChanged);
    connect(m_manager, &TuneManager::dataReady, this, &JsonRpcServer::processData);

}

void JsonRpcServer::start()
{
    m_manager->start();
}

void JsonRpcServer::stop()
{
    m_manager->stop();
}

bool JsonRpcServer::tuneAvailable()
{
    return m_manager->tuneAvailable();
}

bool JsonRpcServer::sync(QList<Device *> deviceList)
{
    if (!tuneAvailable()) {
        return false;
    }

    QVariantMap message;
    QVariantMap params;
    QVariantList moods;
    QVariantList todos;
    QVariantMap tune;
    foreach (Device* device, deviceList) {
        if (device->deviceClassId() == moodDeviceClassId) {
            QVariantMap mood;
            mood.insert("name", device->paramValue("name"));
            mood.insert("deviceId", device->id());
            mood.insert("position", device->paramValue("position"));
            mood.insert("icon", device->paramValue("icon"));
            QVariantMap states;
            states.insert("value", device->stateValue(valueStateTypeId).toInt());
            states.insert("active", device->stateValue(activeStateTypeId).toBool());
            mood.insert("states", states);
            moods.append(mood);
        }
        if (device->deviceClassId() == todoDeviceClassId) {
            QVariantMap todo;
            todo.insert("name", device->paramValue("name"));
            todo.insert("deviceId", device->id());
            todo.insert("position", device->paramValue("position"));
            todo.insert("icon", device->paramValue("icon"));
            todos.append(todo);
        }
        if (device->deviceClassId() == tuneDeviceClassId) {
            tune.insert("name", device->paramValue("name"));
            tune.insert("deviceId", device->id());
            tune.insert("brightness", device->stateValue(brightnessStateTypeId).toInt());
            tune.insert("power", device->stateValue(powerStateTypeId).toBool());
        }
    }
    m_id++;
    params.insert("moods", moods);
    params.insert("todos", todos);
    params.insert("tune", tune);
    message.insert("method", "Items.Sync");
    message.insert("id", m_id);
    message.insert("params", params);

    m_requests.insert(m_id, message);

    QJsonDocument jsonDoc = QJsonDocument::fromVariant(message);
    qCDebug(dcTune) << jsonDoc.toJson();
    m_manager->sendData(jsonDoc.toJson(QJsonDocument::Compact));
    return true;
}

void JsonRpcServer::executeAction(Device *device, const Action &action)
{
    QVariantMap message;
    QVariantMap params;

    params.insert("deviceId", device->id());
    params.insert("actionId", action.id());

    if (device->deviceClassId() == moodDeviceClassId) {
        message.insert("method", "Mood.ExecuteAction");
        if (action.actionTypeId() == valueActionTypeId) {
            params.insert("value", action.param("value").value().toInt());
            params.insert("active", device->stateValue(activeStateTypeId).toBool());
        } else if (action.actionTypeId() == activeActionTypeId) {
            params.insert("value", device->stateValue(valueStateTypeId).toInt());
            params.insert("active", action.param("active").value().toBool());
        }
    } else if(device->deviceClassId() == tuneDeviceClassId) {
        message.insert("method", "Tune.ExecuteAction");
        if (action.actionTypeId() == brightnessActionTypeId) {
            params.insert("brightness", action.param("brightness").value().toInt());
            params.insert("power", device->stateValue(powerStateTypeId).toBool());
        } else if (action.actionTypeId() == powerActionTypeId) {
            params.insert("brightness", device->stateValue(brightnessStateTypeId).toInt());
            params.insert("power", action.param("power").value().toBool());
        }
    }

    m_id++;
    message.insert("id", m_id);
    message.insert("params", params);

    m_requests.insert(m_id, message);

    QJsonDocument jsonDoc = QJsonDocument::fromVariant(message);
    qCDebug(dcTune) << jsonDoc.toJson();
    m_manager->sendData(jsonDoc.toJson(QJsonDocument::Compact));
}

QByteArray JsonRpcServer::formatResponse(int commandId, const QVariantMap &responseParams)
{
    QVariantMap responseMap;
    responseMap.insert("id", commandId);
    responseMap.insert("success", true);
    responseMap.insert("params", responseParams);
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(responseMap);
    return jsonDoc.toJson(QJsonDocument::Compact);
}

QByteArray JsonRpcServer::formatErrorResponse(int commandId, const QString &errorMessage)
{
    QVariantMap responseMap;
    responseMap.insert("id", commandId);
    responseMap.insert("success", false);
    responseMap.insert("error", errorMessage);
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(responseMap);
    return jsonDoc.toJson(QJsonDocument::Compact);
}

void JsonRpcServer::handleResponse(const QVariantMap &response)
{
    int responseId = response.value("id").toInt();

    if (!m_requests.contains(responseId)) {
        qCWarning(dcTune) << "got a response without a corresponding request!!!!";
        return;
    }

    // remove it request because we have a response now...
    QVariantMap request = m_requests.take(responseId);

    // Note: maby we have to do something if any request fails
    if (!response.value("success").toBool()) {
        qCWarning(dcTune) << "error for request:" << request << response.value("error").toString();
    }

    // check if this is a response to an action execution
    if (response.contains("actionId")) {
        emit gotActionResponse(response);
    }
}

void JsonRpcServer::processData(const QByteArray &data)
{
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &error);

    if(error.error != QJsonParseError::NoError) {
        qCWarning(dcTune) << "failed to parse data" << data << ":" << error.errorString();
        return;
    }

    QVariantMap message = jsonDoc.toVariant().toMap();

    // check if the message has an id
    if (!message.contains("id")) {
        qCWarning(dcTune) << "message does not contain a valid id" << message;
        return;
    }

    // if this is a response message to a request
    if (message.contains("success") && !message.contains("method")) {
        handleResponse(message);
        return;
    }

    // otherwise we need a method
    if (!message.contains("method")) {
        qCWarning(dcTune) << "message does not contain a valid method" << message;
        return;
    }

    int commandId = message.value("id").toInt();

    if (message.value("method").toString() == "Mood.SyncStates") {
        emit gotMoodSync(message.value("params").toMap());
        m_manager->sendData(formatResponse(commandId));
    }

    if (message.value("method").toString() == "Tune.SyncStates") {
        emit gotTuneSync(message.value("params").toMap());
        m_manager->sendData(formatResponse(commandId));
    }

    if (message.value("method").toString() == "Todo.OnPressed") {
        emit gotTodoEvent(message.value("params").toMap());
        m_manager->sendData(formatResponse(commandId));
    }

}
