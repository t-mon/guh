/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2015 Simon Stuerz <simon.stuerz@guh.guru>                *
 *  Copyright (C) 2014 Michael Zanetti <michael_zanetti@gmx.net>           *
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

#include "jsontypes.h"

#include "plugin/device.h"
#include "devicemanager.h"
#include "guhcore.h"
#include "ruleengine.h"
#include "loggingcategories.h"

#include <QStringList>
#include <QJsonDocument>
#include <QDebug>
#include <QMetaEnum>

namespace guhserver {

bool JsonTypes::s_initialized = false;
QString JsonTypes::s_lastError;

QVariantList JsonTypes::s_basicType;
QVariantList JsonTypes::s_basicTag;
QVariantList JsonTypes::s_deviceIcon;
QVariantList JsonTypes::s_stateOperator;
QVariantList JsonTypes::s_valueOperator;
QVariantList JsonTypes::s_inputType;
QVariantList JsonTypes::s_unit;
QVariantList JsonTypes::s_createMethod;
QVariantList JsonTypes::s_setupMethod;
QVariantList JsonTypes::s_removePolicy;
QVariantList JsonTypes::s_deviceError;
QVariantList JsonTypes::s_ruleError;
QVariantList JsonTypes::s_loggingError;
QVariantList JsonTypes::s_loggingSource;
QVariantList JsonTypes::s_loggingLevel;
QVariantList JsonTypes::s_loggingEventType;
QVariantList JsonTypes::s_authenticationError;

QVariantMap JsonTypes::s_paramType;
QVariantMap JsonTypes::s_param;
QVariantMap JsonTypes::s_ruleAction;
QVariantMap JsonTypes::s_ruleActionParam;
QVariantMap JsonTypes::s_paramDescriptor;
QVariantMap JsonTypes::s_stateType;
QVariantMap JsonTypes::s_state;
QVariantMap JsonTypes::s_stateDescriptor;
QVariantMap JsonTypes::s_stateEvaluator;
QVariantMap JsonTypes::s_eventType;
QVariantMap JsonTypes::s_event;
QVariantMap JsonTypes::s_eventDescriptor;
QVariantMap JsonTypes::s_actionType;
QVariantMap JsonTypes::s_action;
QVariantMap JsonTypes::s_plugin;
QVariantMap JsonTypes::s_vendor;
QVariantMap JsonTypes::s_deviceClass;
QVariantMap JsonTypes::s_device;
QVariantMap JsonTypes::s_deviceDescriptor;
QVariantMap JsonTypes::s_rule;
QVariantMap JsonTypes::s_ruleDescription;
QVariantMap JsonTypes::s_logEntry;
QVariantMap JsonTypes::s_authorizedConnection;
QVariantMap JsonTypes::s_user;

void JsonTypes::init()
{
    // BasicTypes
    s_basicType = enumToStrings(JsonTypes::staticMetaObject, "BasicType");
    s_stateOperator = enumToStrings(Types::staticMetaObject, "StateOperator");
    s_valueOperator = enumToStrings(Types::staticMetaObject, "ValueOperator");
    s_inputType = enumToStrings(Types::staticMetaObject, "InputType");
    s_unit = enumToStrings(Types::staticMetaObject, "Unit");
    s_createMethod = enumToStrings(DeviceClass::staticMetaObject, "CreateMethod");
    s_setupMethod = enumToStrings(DeviceClass::staticMetaObject, "SetupMethod");
    s_basicTag = enumToStrings(DeviceClass::staticMetaObject, "BasicTag");
    s_deviceIcon = enumToStrings(DeviceClass::staticMetaObject, "DeviceIcon");
    s_removePolicy = enumToStrings(RuleEngine::staticMetaObject, "RemovePolicy");
    s_deviceError = enumToStrings(DeviceManager::staticMetaObject, "DeviceError");
    s_ruleError = enumToStrings(RuleEngine::staticMetaObject, "RuleError");
    s_loggingError = enumToStrings(Logging::staticMetaObject, "LoggingError");
    s_loggingSource = enumToStrings(Logging::staticMetaObject, "LoggingSource");
    s_loggingLevel = enumToStrings(Logging::staticMetaObject, "LoggingLevel");
    s_loggingEventType = enumToStrings(Logging::staticMetaObject, "LoggingEventType");
    s_authenticationError = enumToStrings(AuthenticationManager::staticMetaObject, "AuthenticationError");

    // ParamType
    s_paramType.insert("name", basicTypeToString(String));
    s_paramType.insert("type", basicTypeRef());
    s_paramType.insert("o:defaultValue", basicTypeToString(Variant));
    s_paramType.insert("o:minValue", basicTypeToString(Variant));
    s_paramType.insert("o:maxValue", basicTypeToString(Variant));
    s_paramType.insert("o:allowedValues", QVariantList() << basicTypeToString(Variant));
    s_paramType.insert("o:inputType", inputTypeRef());
    s_paramType.insert("o:unit", unitRef());
    s_paramType.insert("o:readOnly", basicTypeToString(Bool));

    // Param
    s_param.insert("name", basicTypeToString(String));
    s_param.insert("value", basicTypeRef());

    // RuleAction
    s_ruleAction.insert("actionTypeId", basicTypeToString(Uuid));
    s_ruleAction.insert("deviceId", basicTypeToString(Uuid));
    s_ruleAction.insert("o:ruleActionParams", QVariantList() << ruleActionParamRef());

    // RuleActionParam
    s_ruleActionParam.insert("name", basicTypeToString(String));
    s_ruleActionParam.insert("o:value", basicTypeRef());
    s_ruleActionParam.insert("o:eventTypeId", basicTypeToString(Uuid));
    s_ruleActionParam.insert("o:eventParamName", basicTypeToString(String));

    // ParamDescriptor
    s_paramDescriptor.insert("name", basicTypeToString(String));
    s_paramDescriptor.insert("value", basicTypeRef());
    s_paramDescriptor.insert("operator", valueOperatorRef());

    // StateType
    s_stateType.insert("id", basicTypeToString(Uuid));
    s_stateType.insert("name", basicTypeToString(String));
    s_stateType.insert("type", basicTypeRef());
    s_stateType.insert("defaultValue", basicTypeToString(Variant));
    s_stateType.insert("o:unit", unitRef());
    s_stateType.insert("o:minValue", basicTypeToString(Variant));
    s_stateType.insert("o:maxValue", basicTypeToString(Variant));
    s_stateType.insert("o:possibleValues", QVariantList() << basicTypeToString(Variant));

    // State
    s_state.insert("stateTypeId", basicTypeToString(Uuid));
    s_state.insert("deviceId", basicTypeToString(Uuid));
    s_state.insert("value", basicTypeToString(Variant));

    // StateDescriptor
    s_stateDescriptor.insert("stateTypeId", basicTypeToString(Uuid));
    s_stateDescriptor.insert("deviceId", basicTypeToString(Uuid));
    s_stateDescriptor.insert("value", basicTypeToString(Variant));
    s_stateDescriptor.insert("operator", valueOperatorRef());

    // StateEvaluator
    s_stateEvaluator.insert("o:stateDescriptor", stateDescriptorRef());
    s_stateEvaluator.insert("o:childEvaluators", QVariantList() << stateEvaluatorRef());
    s_stateEvaluator.insert("o:operator", stateOperatorRef());

    // EventType
    s_eventType.insert("id", basicTypeToString(Uuid));
    s_eventType.insert("name", basicTypeToString(String));
    s_eventType.insert("paramTypes", QVariantList() << paramTypeRef());

    // Event
    s_event.insert("eventTypeId", basicTypeToString(Uuid));
    s_event.insert("deviceId", basicTypeToString(Uuid));
    s_event.insert("o:params", QVariantList() << paramRef());

    // EventDescriptor
    s_eventDescriptor.insert("eventTypeId", basicTypeToString(Uuid));
    s_eventDescriptor.insert("deviceId", basicTypeToString(Uuid));
    s_eventDescriptor.insert("o:paramDescriptors", QVariantList() << paramDescriptorRef());

    // ActionType
    s_actionType.insert("id", basicTypeToString(Uuid));
    s_actionType.insert("name", basicTypeToString(Uuid));
    s_actionType.insert("paramTypes", QVariantList() << paramTypeRef());

    // Action
    s_action.insert("actionTypeId", basicTypeToString(Uuid));
    s_action.insert("deviceId", basicTypeToString(Uuid));
    s_action.insert("o:params", QVariantList() << paramRef());

    // Pugin
    s_plugin.insert("id", basicTypeToString(Uuid));
    s_plugin.insert("name", basicTypeToString(String));
    s_plugin.insert("params", QVariantList() << paramRef());

    // Vendor
    s_vendor.insert("id", basicTypeToString(Uuid));
    s_vendor.insert("name", basicTypeToString(String));

    // DeviceClass
    s_deviceClass.insert("id", basicTypeToString(Uuid));
    s_deviceClass.insert("vendorId", basicTypeToString(Uuid));
    s_deviceClass.insert("pluginId", basicTypeToString(Uuid));
    s_deviceClass.insert("name", basicTypeToString(String));
    s_deviceClass.insert("deviceIcon", deviceIconRef());
    s_deviceClass.insert("basicTags", QVariantList() << basicTagRef());
    s_deviceClass.insert("stateTypes", QVariantList() << stateTypeRef());
    s_deviceClass.insert("eventTypes", QVariantList() << eventTypeRef());
    s_deviceClass.insert("actionTypes", QVariantList() << actionTypeRef());
    s_deviceClass.insert("paramTypes", QVariantList() << paramTypeRef());
    s_deviceClass.insert("discoveryParamTypes", QVariantList() << paramTypeRef());
    s_deviceClass.insert("setupMethod", setupMethodRef());
    s_deviceClass.insert("createMethods", QVariantList() << createMethodRef());

    // Device
    s_device.insert("id", basicTypeToString(Uuid));
    s_device.insert("deviceClassId", basicTypeToString(Uuid));
    s_device.insert("name", basicTypeToString(String));
    s_device.insert("params", QVariantList() << paramRef());
    QVariantMap stateValues;
    stateValues.insert("stateTypeId", basicTypeToString(Uuid));
    stateValues.insert("value", basicTypeToString(Variant));
    s_device.insert("states", QVariantList() << stateValues);
    s_device.insert("setupComplete", basicTypeToString(Bool));
    s_device.insert("o:parentId", basicTypeToString(Uuid));

    // DeviceDescription
    s_deviceDescriptor.insert("id", basicTypeToString(Uuid));
    s_deviceDescriptor.insert("title", basicTypeToString(String));
    s_deviceDescriptor.insert("description", basicTypeToString(String));

    // Rule
    s_rule.insert("id", basicTypeToString(Uuid));
    s_rule.insert("name", basicTypeToString(String));
    s_rule.insert("enabled", basicTypeToString(Bool));
    s_rule.insert("executable", basicTypeToString(Bool));
    s_rule.insert("active", basicTypeToString(Bool));
    s_rule.insert("eventDescriptors", QVariantList() << eventDescriptorRef());
    s_rule.insert("actions", QVariantList() << ruleActionRef());
    s_rule.insert("exitActions", QVariantList() << ruleActionRef());
    s_rule.insert("stateEvaluator", stateEvaluatorRef());

    // RuleDescription
    s_ruleDescription.insert("id", basicTypeToString(Uuid));
    s_ruleDescription.insert("name", basicTypeToString(String));
    s_ruleDescription.insert("enabled", basicTypeToString(Bool));
    s_ruleDescription.insert("active", basicTypeToString(Bool));
    s_ruleDescription.insert("executable", basicTypeToString(Bool));

    // LogEntry
    s_logEntry.insert("timestamp", basicTypeToString(Int));
    s_logEntry.insert("loggingLevel", loggingLevelRef());
    s_logEntry.insert("source", loggingSourceRef());
    s_logEntry.insert("o:typeId", basicTypeToString(Uuid));
    s_logEntry.insert("o:deviceId", basicTypeToString(Uuid));
    s_logEntry.insert("o:value", basicTypeToString(String));
    s_logEntry.insert("o:active", basicTypeToString(Bool));
    s_logEntry.insert("o:eventType", loggingEventTypeRef());
    s_logEntry.insert("o:errorCode", basicTypeToString(String));

    // Authorized Connection
    s_authorizedConnection.insert("clientDescription", basicTypeToString(String));
    s_authorizedConnection.insert("token", basicTypeToString(String));
    s_authorizedConnection.insert("lastUpdate", basicTypeToString(Int));
    s_authorizedConnection.insert("userName", basicTypeToString(String));

    // user
    s_user.insert("userId", basicTypeToString(String));
    s_user.insert("userName", basicTypeToString(String));
    s_user.insert("isAdmin", basicTypeToString(Bool));

    s_initialized = true;
}

QPair<bool, QString> JsonTypes::report(bool status, const QString &message)
{
    return qMakePair<bool, QString>(status, message);
}

QVariantList JsonTypes::enumToStrings(const QMetaObject &metaObject, const QString &enumName)
{
    int enumIndex = metaObject.indexOfEnumerator(enumName.toLatin1().data());
    Q_ASSERT_X(enumIndex >= 0, "JsonTypes", QString("Enumerator %1 not found in %2").arg(enumName).arg(metaObject.className()).toLocal8Bit());
    QMetaEnum metaEnum = metaObject.enumerator(enumIndex);

    QVariantList enumStrings;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        enumStrings << metaEnum.valueToKey(metaEnum.value(i));
    }
    return enumStrings;
}

QVariantMap JsonTypes::allTypes()
{
    QVariantMap allTypes;
    allTypes.insert("BasicType", basicType());
    allTypes.insert("BasicTag", basicTag());
    allTypes.insert("ParamType", paramTypeDescription());
    allTypes.insert("InputType", inputType());
    allTypes.insert("Unit", unit());
    allTypes.insert("CreateMethod", createMethod());
    allTypes.insert("SetupMethod", setupMethod());
    allTypes.insert("DeviceIcon", deviceIcon());
    allTypes.insert("ValueOperator", valueOperator());
    allTypes.insert("StateOperator", stateOperator());
    allTypes.insert("RemovePolicy", removePolicy());
    allTypes.insert("DeviceError", deviceError());
    allTypes.insert("RuleError", ruleError());
    allTypes.insert("LoggingError", loggingError());
    allTypes.insert("LoggingLevel", loggingLevel());
    allTypes.insert("LoggingSource", loggingSource());
    allTypes.insert("LoggingEventType", loggingEventType());
    allTypes.insert("AuthenticationError", authenticationError());

    allTypes.insert("StateType", stateTypeDescription());
    allTypes.insert("StateDescriptor", stateDescriptorDescription());
    allTypes.insert("StateEvaluator", stateEvaluatorDescription());
    allTypes.insert("Event", eventDescription());
    allTypes.insert("EventType", eventTypeDescription());
    allTypes.insert("EventDescriptor", eventDescriptorDescription());
    allTypes.insert("ActionType", actionTypeDescription());
    allTypes.insert("Vendor", vendorDescription());
    allTypes.insert("DeviceClass", deviceClassDescription());
    allTypes.insert("Plugin", pluginDescription());
    allTypes.insert("Param", paramDescription());
    allTypes.insert("RuleAction", ruleActionDescription());
    allTypes.insert("RuleActionParam", ruleActionParamDescription());
    allTypes.insert("ParamDescriptor", paramDescriptorDescription());
    allTypes.insert("State", stateDescription());
    allTypes.insert("Device", deviceDescription());
    allTypes.insert("DeviceDescriptor", deviceDescriptorDescription());
    allTypes.insert("Action", actionDescription());
    allTypes.insert("Rule", ruleDescription());
    allTypes.insert("RuleDescription", ruleDescriptionDescription());
    allTypes.insert("LogEntry", logEntryDescription());
    allTypes.insert("AuthorizedConnection", authorizedConnectionDescription());
    allTypes.insert("User", userDescription());
    return allTypes;
}

QVariantMap JsonTypes::packEventType(const EventType &eventType)
{
    QVariantMap variant;
    variant.insert("id", eventType.id());
    variant.insert("name", eventType.name());
    QVariantList paramTypes;
    foreach (const ParamType &paramType, eventType.paramTypes()) {
        paramTypes.append(packParamType(paramType));
    }
    variant.insert("paramTypes", paramTypes);
    return variant;
}

QVariantMap JsonTypes::packEvent(const Event &event)
{
    QVariantMap variant;
    variant.insert("eventTypeId", event.eventTypeId());
    variant.insert("deviceId", event.deviceId());
    QVariantList params;
    foreach (const Param &param, event.params()) {
        params.append(packParam(param));
    }
    variant.insert("params", params);
    return variant;
}

QVariantMap JsonTypes::packEventDescriptor(const EventDescriptor &eventDescriptor)
{
    QVariantMap variant;
    variant.insert("eventTypeId", eventDescriptor.eventTypeId());
    variant.insert("deviceId", eventDescriptor.deviceId());
    QVariantList params;
    foreach (const ParamDescriptor &paramDescriptor, eventDescriptor.paramDescriptors()) {
        params.append(packParamDescriptor(paramDescriptor));
    }
    variant.insert("paramDescriptors", params);
    return variant;
}

QVariantMap JsonTypes::packActionType(const ActionType &actionType)
{
    QVariantMap variantMap;
    variantMap.insert("id", actionType.id());
    variantMap.insert("name", actionType.name());
    QVariantList paramTypes;
    foreach (const ParamType &paramType, actionType.paramTypes()) {
        paramTypes.append(packParamType(paramType));
    }
    variantMap.insert("paramTypes", paramTypes);
    return variantMap;
}

QVariantMap JsonTypes::packAction(const Action &action)
{
    QVariantMap variant;
    variant.insert("actionTypeId", action.actionTypeId());
    variant.insert("deviceId", action.deviceId());
    QVariantList params;
    foreach (const Param &param, action.params()) {
        params.append(packParam(param));
    }
    variant.insert("params", params);
    return variant;
}

QVariantMap JsonTypes::packRuleAction(const RuleAction &ruleAction)
{
    QVariantMap variant;
    variant.insert("actionTypeId", ruleAction.actionTypeId());
    variant.insert("deviceId", ruleAction.deviceId());
    QVariantList params;
    foreach (const RuleActionParam &ruleActionParam, ruleAction.ruleActionParams()) {
        params.append(packRuleActionParam(ruleActionParam));
    }
    variant.insert("ruleActionParams", params);
    return variant;
}

QVariantMap JsonTypes::packRuleActionParam(const RuleActionParam &ruleActionParam)
{
    QVariantMap variantMap;
    variantMap.insert("name", ruleActionParam.name());
    // if this ruleaction param has a valid EventTypeId, there is no value
    if (ruleActionParam.eventTypeId() != EventTypeId()) {
        variantMap.insert("eventTypeId", ruleActionParam.eventTypeId());
        variantMap.insert("eventParamName", ruleActionParam.eventParamName());
    } else {
        variantMap.insert("value", ruleActionParam.value());
    }
    return variantMap;
}

QVariantMap JsonTypes::packState(const State &state)
{
    QVariantMap stateMap;
    stateMap.insert("stateTypeId", state.stateTypeId().toString());
    stateMap.insert("value", state.value());
    return stateMap;
}

QVariantMap JsonTypes::packStateType(const StateType &stateType)
{
    QVariantMap variantMap;
    variantMap.insert("id", stateType.id());
    variantMap.insert("name", stateType.name());
    variantMap.insert("type", basicTypeToString(stateType.type()));
    variantMap.insert("defaultValue", stateType.defaultValue());

    if (stateType.maxValue().isValid())
        variantMap.insert("maxValue", stateType.maxValue());

    if (stateType.minValue().isValid())
        variantMap.insert("minValue", stateType.minValue());

    if (!stateType.possibleValues().isEmpty())
        variantMap.insert("possibleValues", stateType.possibleValues());

    if(stateType.unit() != Types::UnitNone)
        variantMap.insert("unit", s_unit.at(stateType.unit()));

    return variantMap;
}

QVariantMap JsonTypes::packStateDescriptor(const StateDescriptor &stateDescriptor)
{
    QVariantMap variantMap;
    variantMap.insert("stateTypeId", stateDescriptor.stateTypeId().toString());
    variantMap.insert("deviceId", stateDescriptor.deviceId().toString());
    variantMap.insert("value", stateDescriptor.stateValue());
    variantMap.insert("operator", valueOperator().at(stateDescriptor.operatorType()));
    return variantMap;
}

QVariantMap JsonTypes::packStateEvaluator(const StateEvaluator &stateEvaluator)
{
    QVariantMap variantMap;
    if (stateEvaluator.stateDescriptor().isValid()) {
        variantMap.insert("stateDescriptor", packStateDescriptor(stateEvaluator.stateDescriptor()));
    }
    QVariantList childEvaluators;
    foreach (const StateEvaluator &childEvaluator, stateEvaluator.childEvaluators()) {
        childEvaluators.append(packStateEvaluator(childEvaluator));
    }
    qCDebug(dcJsonRpc) << "state operator:" << stateOperator() << stateEvaluator.operatorType();
    variantMap.insert("operator", stateOperator().at(stateEvaluator.operatorType()));
    if (childEvaluators.count() > 0) {
        variantMap.insert("childEvaluators", childEvaluators);
    }

    return variantMap;
}

QVariantMap JsonTypes::packParam(const Param &param)
{
    QVariantMap variantMap;
    variantMap.insert("name", param.name());
    variantMap.insert("value", param.value());
    return variantMap;
}

QVariantMap JsonTypes::packParamDescriptor(const ParamDescriptor &paramDescriptor)
{
    QVariantMap variantMap;
    variantMap.insert("name", paramDescriptor.name());
    variantMap.insert("value", paramDescriptor.value());
    variantMap.insert("operator", s_valueOperator.at(paramDescriptor.operatorType()));
    return variantMap;
}

QVariantMap JsonTypes::packParamType(const ParamType &paramType)
{
    QVariantMap variantMap;
    variantMap.insert("name", paramType.name());
    variantMap.insert("type", basicTypeToString(paramType.type()));
    // optional
    if (paramType.defaultValue().isValid()) {
        variantMap.insert("defaultValue", paramType.defaultValue());
    }
    if (paramType.minValue().isValid()) {
        variantMap.insert("minValue", paramType.minValue());
    }
    if (paramType.maxValue().isValid()) {
        variantMap.insert("maxValue", paramType.maxValue());
    }
    if (!paramType.allowedValues().isEmpty()) {
        variantMap.insert("allowedValues", paramType.allowedValues());
    }
    if (paramType.inputType() != Types::InputTypeNone) {
        variantMap.insert("inputType", s_inputType.at(paramType.inputType()));
    }
    if (paramType.unit() != Types::UnitNone) {
        variantMap.insert("unit", s_unit.at(paramType.unit()));
    }
    // only add if this param is NOT writable
    if (paramType.readOnly()) {
        variantMap.insert("readOnly", paramType.readOnly());
    }
    return variantMap;
}

QVariantMap JsonTypes::packVendor(const Vendor &vendor)
{
    QVariantMap variantMap;
    variantMap.insert("id", vendor.id());
    variantMap.insert("name", vendor.name());
    return variantMap;
}

QVariantMap JsonTypes::packDeviceClass(const DeviceClass &deviceClass)
{
    QVariantMap variant;
    variant.insert("name", deviceClass.name());
    variant.insert("id", deviceClass.id());
    variant.insert("vendorId", deviceClass.vendorId());
    variant.insert("pluginId", deviceClass.pluginId());
    variant.insert("deviceIcon", s_deviceIcon.at(deviceClass.deviceIcon()));

    QVariantList basicTags;
    foreach (const DeviceClass::BasicTag &basicTag, deviceClass.basicTags()) {
        basicTags.append(s_basicTag.at(basicTag));
    }
    QVariantList stateTypes;
    foreach (const StateType &stateType, deviceClass.stateTypes()) {
        stateTypes.append(packStateType(stateType));
    }
    QVariantList eventTypes;
    foreach (const EventType &eventType, deviceClass.eventTypes()) {
        eventTypes.append(packEventType(eventType));
    }
    QVariantList actionTypes;
    foreach (const ActionType &actionType, deviceClass.actionTypes()) {
        actionTypes.append(packActionType(actionType));
    }
    QVariantList paramTypes;
    foreach (const ParamType &paramType, deviceClass.paramTypes()) {
        paramTypes.append(packParamType(paramType));
    }
    QVariantList discoveryParamTypes;
    foreach (const ParamType &paramType, deviceClass.discoveryParamTypes()) {
        discoveryParamTypes.append(packParamType(paramType));
    }

    variant.insert("basicTags", basicTags);
    variant.insert("paramTypes", paramTypes);
    variant.insert("discoveryParamTypes", discoveryParamTypes);
    variant.insert("stateTypes", stateTypes);
    variant.insert("eventTypes", eventTypes);
    variant.insert("actionTypes", actionTypes);
    variant.insert("createMethods", packCreateMethods(deviceClass.createMethods()));
    variant.insert("setupMethod", s_setupMethod.at(deviceClass.setupMethod()));
    return variant;
}

QVariantMap JsonTypes::packPlugin(DevicePlugin *plugin)
{
    QVariantMap pluginMap;
    pluginMap.insert("id", plugin->pluginId());
    pluginMap.insert("name", plugin->pluginName());

    QVariantList params;
    foreach (const ParamType &param, plugin->configurationDescription()) {
        params.append(packParamType(param));
    }
    pluginMap.insert("params", params);

    return pluginMap;
}

QVariantMap JsonTypes::packDevice(Device *device)
{
    QVariantMap variant;
    variant.insert("id", device->id());
    variant.insert("deviceClassId", device->deviceClassId());
    variant.insert("name", device->name());
    QVariantList params;
    foreach (const Param &param, device->params()) {
        params.append(packParam(param));
    }

    if (!device->parentId().isNull())
        variant.insert("parentId", device->parentId());

    variant.insert("params", params);
    variant.insert("states", packDeviceStates(device));
    variant.insert("setupComplete", device->setupComplete());
    return variant;
}

QVariantMap JsonTypes::packDeviceDescriptor(const DeviceDescriptor &descriptor)
{
    QVariantMap variant;
    variant.insert("id", descriptor.id());
    variant.insert("title", descriptor.title());
    variant.insert("description", descriptor.description());
    return variant;
}

QVariantMap JsonTypes::packRule(const Rule &rule)
{
    QVariantMap ruleMap;
    ruleMap.insert("id", rule.id());
    ruleMap.insert("name", rule.name());
    ruleMap.insert("enabled", rule.enabled());
    ruleMap.insert("active", rule.active());
    ruleMap.insert("executable", rule.executable());

    QVariantList eventDescriptorList;
    foreach (const EventDescriptor &eventDescriptor, rule.eventDescriptors()) {
        eventDescriptorList.append(JsonTypes::packEventDescriptor(eventDescriptor));
    }
    ruleMap.insert("eventDescriptors", eventDescriptorList);

    QVariantList actionList;
    foreach (const RuleAction &action, rule.actions()) {
        actionList.append(JsonTypes::packRuleAction(action));
    }
    ruleMap.insert("actions", actionList);

    QVariantList exitActionList;
    foreach (const RuleAction &action, rule.exitActions()) {
        exitActionList.append(JsonTypes::packRuleAction(action));
    }
    ruleMap.insert("exitActions", exitActionList);
    ruleMap.insert("stateEvaluator", JsonTypes::packStateEvaluator(rule.stateEvaluator()));
    return ruleMap;
}

QVariantList JsonTypes::packRules(const QList<Rule> rules)
{
    QVariantList rulesList;
    foreach (const Rule &rule, rules) {
        rulesList.append(JsonTypes::packRule(rule));
    }
    return rulesList;
}

QVariantMap JsonTypes::packRuleDescription(const Rule &rule)
{
    QVariantMap ruleDescriptionMap;
    ruleDescriptionMap.insert("id", rule.id());
    ruleDescriptionMap.insert("name", rule.name());
    ruleDescriptionMap.insert("enabled", rule.enabled());
    ruleDescriptionMap.insert("active", rule.active());
    ruleDescriptionMap.insert("executable", rule.executable());
    return ruleDescriptionMap;
}

QVariantMap JsonTypes::packLogEntry(const LogEntry &logEntry)
{
    QVariantMap logEntryMap;
    logEntryMap.insert("timestamp", logEntry.timestamp().toMSecsSinceEpoch());
    logEntryMap.insert("loggingLevel", s_loggingLevel.at(logEntry.level()));
    logEntryMap.insert("source", s_loggingSource.at(logEntry.source()));
    logEntryMap.insert("eventType", s_loggingEventType.at(logEntry.eventType()));

    if (logEntry.eventType() == Logging::LoggingEventTypeActiveChange) {
        logEntryMap.insert("active", logEntry.active());
    }

    if (logEntry.level() == Logging::LoggingLevelAlert) {
        switch (logEntry.source()) {
        case Logging::LoggingSourceRules:
            logEntryMap.insert("errorCode", s_ruleError.at(logEntry.errorCode()));
            break;
        case Logging::LoggingSourceActions:
        case Logging::LoggingSourceEvents:
        case Logging::LoggingSourceStates:
            logEntryMap.insert("errorCode", s_deviceError.at(logEntry.errorCode()));
            break;
        case Logging::LoggingSourceSystem:
            // FIXME: Update this once we support error codes for the general system
            //            logEntryMap.insert("errorCode", "");
            break;
        }
    }

    switch (logEntry.source()) {
    case Logging::LoggingSourceActions:
    case Logging::LoggingSourceEvents:
    case Logging::LoggingSourceStates:
        logEntryMap.insert("typeId", logEntry.typeId().toString());
        logEntryMap.insert("deviceId", logEntry.deviceId().toString());
        logEntryMap.insert("value", logEntry.value());
        break;
    case Logging::LoggingSourceSystem:
        logEntryMap.insert("active", logEntry.active());
        break;
    case Logging::LoggingSourceRules:
        logEntryMap.insert("typeId", logEntry.typeId().toString());
        break;
    }

    return logEntryMap;
}

QVariantList JsonTypes::packCreateMethods(DeviceClass::CreateMethods createMethods)
{
    QVariantList ret;
    if (createMethods.testFlag(DeviceClass::CreateMethodUser)) {
        ret << "CreateMethodUser";
    }
    if (createMethods.testFlag(DeviceClass::CreateMethodAuto)) {
        ret << "CreateMethodAuto";
    }
    if (createMethods.testFlag(DeviceClass::CreateMethodDiscovery)) {
        ret << "CreateMethodDiscovery";
    }
    return ret;
}

QVariantMap JsonTypes::packAuthorizedConnection(const AuthorizedConnection &connection)
{
    QVariantMap connectionMap;
    connectionMap.insert("clientDescription", connection.clientDescription());
    connectionMap.insert("token", connection.token());
    connectionMap.insert("lastUpdate", connection.lastLogin());
    connectionMap.insert("userName", GuhCore::instance()->authenticationManager()->getUser(connection.userId()).userName());
    return connectionMap;
}

QVariantMap JsonTypes::packUser(const User &user)
{
    QVariantMap userMap;
    userMap.insert("userId", user.userId());
    userMap.insert("userName", user.userName());
    userMap.insert("isAdmin", user.isAdmin());
    return userMap;
}

QVariantList JsonTypes::packSupportedVendors()
{
    QVariantList supportedVendors;
    foreach (const Vendor &vendor, GuhCore::instance()->deviceManager()->supportedVendors()) {
        supportedVendors.append(packVendor(vendor));
    }
    return supportedVendors;
}

QVariantList JsonTypes::packSupportedDevices(const VendorId &vendorId)
{
    QVariantList supportedDeviceList;
    foreach (const DeviceClass &deviceClass, GuhCore::instance()->deviceManager()->supportedDevices(vendorId)) {
        supportedDeviceList.append(packDeviceClass(deviceClass));
    }
    return supportedDeviceList;
}

QVariantList JsonTypes::packConfiguredDevices()
{
    QVariantList configuredDeviceList;
    foreach (Device *device, GuhCore::instance()->deviceManager()->configuredDevices()) {
        configuredDeviceList.append(packDevice(device));
    }
    return configuredDeviceList;
}

QVariantList JsonTypes::packDeviceStates(Device *device)
{
    DeviceClass deviceClass = GuhCore::instance()->deviceManager()->findDeviceClass(device->deviceClassId());
    QVariantList stateValues;
    foreach (const StateType &stateType, deviceClass.stateTypes()) {
        QVariantMap stateValue;
        stateValue.insert("stateTypeId", stateType.id().toString());
        stateValue.insert("value", device->stateValue(stateType.id()));
        stateValues.append(stateValue);
    }
    return stateValues;
}

QVariantList JsonTypes::packDeviceDescriptors(const QList<DeviceDescriptor> deviceDescriptors)
{
    QVariantList deviceDescriptorList;
    foreach (const DeviceDescriptor &deviceDescriptor, deviceDescriptors) {
        deviceDescriptorList.append(JsonTypes::packDeviceDescriptor(deviceDescriptor));
    }
    return deviceDescriptorList;
}

QVariantList JsonTypes::packRuleDescriptions()
{
    QVariantList rulesList;
    foreach (const Rule &rule, GuhCore::instance()->ruleEngine()->rules()) {
        rulesList.append(JsonTypes::packRuleDescription(rule));
    }
    return rulesList;
}

QVariantList JsonTypes::packRuleDescriptions(const QList<Rule> &rules)
{
    QVariantList rulesList;
    foreach (const Rule &rule, rules) {
        rulesList.append(JsonTypes::packRuleDescription(rule));
    }
    return rulesList;
}

QVariantList JsonTypes::packActionTypes(const DeviceClass &deviceClass)
{
    QVariantList actionTypes;
    foreach (const ActionType &actionType, deviceClass.actionTypes()) {
        actionTypes.append(JsonTypes::packActionType(actionType));
    }
    return actionTypes;
}

QVariantList JsonTypes::packStateTypes(const DeviceClass &deviceClass)
{
    QVariantList stateTypes;
    foreach (const StateType &stateType, deviceClass.stateTypes()) {
        stateTypes.append(JsonTypes::packStateType(stateType));
    }
    return stateTypes;
}

QVariantList JsonTypes::packEventTypes(const DeviceClass &deviceClass)
{
    QVariantList eventTypes;
    foreach (const EventType &eventType, deviceClass.eventTypes()) {
        eventTypes.append(JsonTypes::packEventType(eventType));
    }
    return eventTypes;
}

QVariantList JsonTypes::packPlugins()
{
    QVariantList pluginsList;
    foreach (DevicePlugin *plugin, GuhCore::instance()->deviceManager()->plugins()) {
        QVariantMap pluginMap = packPlugin(plugin);
        pluginsList.append(pluginMap);
    }
    return pluginsList;
}

QString JsonTypes::basicTypeToString(const QVariant::Type &type)
{
    switch (type) {
    case QVariant::Uuid:
        return "Uuid";
        break;
    case QVariant::String:
        return "String";
        break;
    case QVariant::Int:
        return "Int";
        break;
    case QVariant::UInt:
        return "UInt";
        break;
    case QVariant::Double:
        return "Double";
        break;
    case QVariant::Bool:
        return "Bool";
        break;
    case QVariant::Color:
        return "Color";
        break;
    default:
        return QVariant::typeToName(type);
        break;
    }
}

Param JsonTypes::unpackParam(const QVariantMap &paramMap)
{
    if (paramMap.keys().count() == 0) {
        return Param();
    }
    QString name = paramMap.value("name").toString();
    QVariant value = paramMap.value("value");
    return Param(name, value);
}

ParamList JsonTypes::unpackParams(const QVariantList &paramList)
{
    ParamList params;
    foreach (const QVariant &paramVariant, paramList) {
        params.append(unpackParam(paramVariant.toMap()));
    }
    return params;
}

RuleActionParam JsonTypes::unpackRuleActionParam(const QVariantMap &ruleActionParamMap)
{
    if (ruleActionParamMap.keys().count() == 0)
        return RuleActionParam();

    QString name = ruleActionParamMap.value("name").toString();
    QVariant value = ruleActionParamMap.value("value");
    EventTypeId eventTypeId = EventTypeId(ruleActionParamMap.value("eventTypeId").toString());
    QString eventParamName = ruleActionParamMap.value("eventParamName").toString();
    return RuleActionParam(name, value, eventTypeId, eventParamName);
}

RuleActionParamList JsonTypes::unpackRuleActionParams(const QVariantList &ruleActionParamList)
{
    RuleActionParamList ruleActionParams;
    foreach (const QVariant &paramVariant, ruleActionParamList) {
        ruleActionParams.append(unpackRuleActionParam(paramVariant.toMap()));
    }
    return ruleActionParams;
}

ParamDescriptor JsonTypes::unpackParamDescriptor(const QVariantMap &paramMap)
{
    ParamDescriptor param(paramMap.value("name").toString(), paramMap.value("value"));
    QString operatorString = paramMap.value("operator").toString();

    QMetaObject metaObject = Types::staticMetaObject;
    int enumIndex = metaObject.indexOfEnumerator("ValueOperator");
    QMetaEnum metaEnum = metaObject.enumerator(enumIndex);
    param.setOperatorType((Types::ValueOperator)metaEnum.keyToValue(operatorString.toLatin1().data()));
    return param;
}

QList<ParamDescriptor> JsonTypes::unpackParamDescriptors(const QVariantList &paramList)
{
    QList<ParamDescriptor> params;
    foreach (const QVariant &paramVariant, paramList) {
        params.append(unpackParamDescriptor(paramVariant.toMap()));
    }
    return params;
}

EventDescriptor JsonTypes::unpackEventDescriptor(const QVariantMap &eventDescriptorMap)
{
    EventTypeId eventTypeId(eventDescriptorMap.value("eventTypeId").toString());
    DeviceId eventDeviceId(eventDescriptorMap.value("deviceId").toString());
    QList<ParamDescriptor> eventParams = JsonTypes::unpackParamDescriptors(eventDescriptorMap.value("paramDescriptors").toList());
    EventDescriptor eventDescriptor(eventTypeId, eventDeviceId, eventParams);
    return eventDescriptor;
}

StateEvaluator JsonTypes::unpackStateEvaluator(const QVariantMap &stateEvaluatorMap)
{
    StateEvaluator ret(unpackStateDescriptor(stateEvaluatorMap.value("stateDescriptor").toMap()));
    if (stateEvaluatorMap.contains("operator")) {
        ret.setOperatorType((Types::StateOperator)s_stateOperator.indexOf(stateEvaluatorMap.value("operator").toString()));
    }
    QList<StateEvaluator> childEvaluators;
    foreach (const QVariant &childEvaluator, stateEvaluatorMap.value("childEvaluators").toList()) {
        childEvaluators.append(unpackStateEvaluator(childEvaluator.toMap()));
    }
    ret.setChildEvaluators(childEvaluators);
    return ret;
}

StateDescriptor JsonTypes::unpackStateDescriptor(const QVariantMap &stateDescriptorMap)
{
    StateTypeId stateTypeId(stateDescriptorMap.value("stateTypeId").toString());
    DeviceId deviceId(stateDescriptorMap.value("deviceId").toString());
    QVariant value = stateDescriptorMap.value("value");
    Types::ValueOperator operatorType = (Types::ValueOperator)s_valueOperator.indexOf(stateDescriptorMap.value("operator").toString());
    StateDescriptor stateDescriptor(stateTypeId, deviceId, value, operatorType);
    return stateDescriptor;
}

LogFilter JsonTypes::unpackLogFilter(const QVariantMap &logFilterMap)
{
    LogFilter filter;
    if (logFilterMap.contains("timeFilters")) {
        QVariantList timeFilters = logFilterMap.value("timeFilters").toList();
        foreach (const QVariant &timeFilter, timeFilters) {
            QVariantMap timeFilterMap = timeFilter.toMap();
            QDateTime startDate; QDateTime endDate;
            if (timeFilterMap.contains("startDate")) {
                startDate = QDateTime::fromTime_t(timeFilterMap.value("startDate").toInt());
            }
            if (timeFilterMap.contains("endDate")) {
                endDate = QDateTime::fromTime_t(timeFilterMap.value("endDate").toInt());
            }
            filter.addTimeFilter(startDate, endDate);
        }
    }

    if (logFilterMap.contains("loggingSources")) {
        QVariantList loggingSources = logFilterMap.value("loggingSources").toList();
        foreach (const QVariant &source, loggingSources) {
            filter.addLoggingSource((Logging::LoggingSource)s_loggingSource.indexOf(source.toString()));
        }
    }
    if (logFilterMap.contains("loggingLevels")) {
        QVariantList loggingLevels = logFilterMap.value("loggingLevels").toList();
        foreach (const QVariant &level, loggingLevels) {
            filter.addLoggingLevel((Logging::LoggingLevel)s_loggingLevel.indexOf(level.toString()));
        }
    }
    if (logFilterMap.contains("eventTypes")) {
        QVariantList eventTypes = logFilterMap.value("eventTypes").toList();
        foreach (const QVariant &eventType, eventTypes) {
            filter.addLoggingEventType((Logging::LoggingEventType)s_loggingEventType.indexOf(eventType.toString()));
        }
    }
    if (logFilterMap.contains("typeIds")) {
        QVariantList typeIds = logFilterMap.value("typeIds").toList();
        foreach (const QVariant &typeId, typeIds) {
            filter.addTypeId(typeId.toUuid());
        }
    }
    if (logFilterMap.contains("deviceIds")) {
        QVariantList deviceIds = logFilterMap.value("deviceIds").toList();
        foreach (const QVariant &deviceId, deviceIds) {
            filter.addDeviceId(DeviceId(deviceId.toString()));
        }
    }
    if (logFilterMap.contains("values")) {
        QVariantList values = logFilterMap.value("values").toList();
        foreach (const QVariant &value, values) {
            filter.addValue(value.toString());
        }
    }

    return filter;
}

QPair<bool, QString> JsonTypes::validateMap(const QVariantMap &templateMap, const QVariantMap &map)
{
    s_lastError.clear();

    // Make sure all values defined in the template are around
    foreach (const QString &key, templateMap.keys()) {
        QString strippedKey = key;
        strippedKey.remove(QRegExp("^o:"));
        if (!key.startsWith("o:") && !map.contains(strippedKey)) {
            qCWarning(dcJsonRpc) << "*** missing key" << key;
            qCWarning(dcJsonRpc) << "Expected:      " << templateMap;
            qCWarning(dcJsonRpc) << "Got:           " << map;
            QJsonDocument jsonDoc = QJsonDocument::fromVariant(map);
            return report(false, QString("Missing key %1 in %2").arg(key).arg(QString(jsonDoc.toJson())));
        }
        if (map.contains(strippedKey)) {
            QPair<bool, QString> result = validateVariant(templateMap.value(key), map.value(strippedKey));
            if (!result.first) {
                qCWarning(dcJsonRpc) << "Object not matching template" << templateMap.value(key) << map.value(strippedKey);
                return result;
            }
        }
    }

    // Make sure there aren't any other parameters than the allowed ones
    foreach (const QString &key, map.keys()) {
        QString optKey = "o:" + key;

        if (!templateMap.contains(key) && !templateMap.contains(optKey)) {
            qCWarning(dcJsonRpc) << "Forbidden param" << key << "in params";
            QJsonDocument jsonDoc = QJsonDocument::fromVariant(map);
            return report(false, QString("Forbidden key \"%1\" in %2").arg(key).arg(QString(jsonDoc.toJson())));
        }
    }

    return report(true, "");
}

QPair<bool, QString> JsonTypes::validateProperty(const QVariant &templateValue, const QVariant &value)
{
    QString strippedTemplateValue = templateValue.toString();

    if (strippedTemplateValue == JsonTypes::basicTypeToString(JsonTypes::Variant)) {
        return report(true, "");
    }
    if (strippedTemplateValue == JsonTypes::basicTypeToString(JsonTypes::Uuid)) {
        QString errorString = QString("Param %1 is not a uuid.").arg(value.toString());
        return report(value.canConvert(QVariant::Uuid), errorString);
    }
    if (strippedTemplateValue == JsonTypes::basicTypeToString(JsonTypes::String)) {
        QString errorString = QString("Param %1 is not a string.").arg(value.toString());
        return report(value.canConvert(QVariant::String), errorString);
    }
    if (strippedTemplateValue == JsonTypes::basicTypeToString(JsonTypes::Bool)) {
        QString errorString = QString("Param %1 is not a bool.").arg(value.toString());
        return report(value.canConvert(QVariant::Bool), errorString);
    }
    if (strippedTemplateValue == JsonTypes::basicTypeToString(JsonTypes::Int)) {
        QString errorString = QString("Param %1 is not a int.").arg(value.toString());
        return report(value.canConvert(QVariant::Int), errorString);
    }
    qCWarning(dcJsonRpc) << QString("Unhandled property type: %1 (expected: %2)").arg(value.toString()).arg(strippedTemplateValue);
    QString errorString = QString("Unhandled property type: %1 (expected: %2)").arg(value.toString()).arg(strippedTemplateValue);
    return report(false, errorString);
}

QPair<bool, QString> JsonTypes::validateList(const QVariantList &templateList, const QVariantList &list)
{
    Q_ASSERT(templateList.count() == 1);
    QVariant entryTemplate = templateList.first();

    for (int i = 0; i < list.count(); ++i) {
        QVariant listEntry = list.at(i);
        QPair<bool, QString> result = validateVariant(entryTemplate, listEntry);
        if (!result.first) {
            qCWarning(dcJsonRpc) << "List entry not matching template";
            return result;
        }
    }
    return report(true, "");
}

QPair<bool, QString> JsonTypes::validateVariant(const QVariant &templateVariant, const QVariant &variant)
{
    switch(templateVariant.type()) {
    case QVariant::String:
        if (templateVariant.toString().startsWith("$ref:")) {
            QString refName = templateVariant.toString();
            if (refName == actionRef()) {
                QPair<bool, QString> result = validateMap(actionDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Error validating action";
                    return result;
                }
            } else if (refName == authorizedConnectionRef()) {
                QPair<bool, QString> result = validateMap(authorizedConnectionDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Error validating authorized connection.";
                    return result;
                }
            } else if (refName == userRef()) {
                QPair<bool, QString> result = validateMap(userDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Error validating user.";
                    return result;
                }
            } else if (refName == eventRef()) {
                QPair<bool, QString> result = validateMap(eventDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Event not valid";
                    return result;
                }
            } else if (refName == paramRef()) {
                if (!variant.canConvert(QVariant::Map)) {
                    report(false, "Param not valid. Should be a map.");
                }
            } else if (refName == paramDescriptorRef()) {
                QPair<bool, QString> result = validateMap(paramDescriptorDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "ParamDescriptor not valid";
                    return result;
                }
            } else if (refName == deviceRef()) {
                QPair<bool, QString> result = validateMap(deviceDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Device not valid";
                    return result;
                }
            } else if (refName == deviceDescriptorRef()) {
                QPair<bool, QString> result = validateMap(deviceDescriptorDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Devicedescriptor not valid";
                    return result;
                }
            } else if (refName == vendorRef()) {
                QPair<bool, QString> result = validateMap(vendorDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Value not allowed in" << vendorRef();
                }
            } else if (refName == deviceClassRef()) {
                QPair<bool, QString> result = validateMap(deviceClassDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Device class not valid";
                    return result;
                }
            } else if (refName == paramTypeRef()) {
                QPair<bool, QString> result = validateMap(paramTypeDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Param types not matching";
                    return result;
                }
            } else if (refName == ruleActionRef()) {
                QPair<bool, QString> result = validateMap(ruleActionDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "RuleAction type not matching";
                    return result;
                }
            } else if (refName == ruleActionParamRef()) {
                QPair<bool, QString> result = validateMap(ruleActionParamDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "RuleActionParam type not matching";
                    return result;
                }
            } else if (refName == actionTypeRef()) {
                QPair<bool, QString> result = validateMap(actionTypeDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Action type not matching";
                    return result;
                }
            } else if (refName == eventTypeRef()) {
                QPair<bool, QString> result = validateMap(eventTypeDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Event type not matching";
                    return result;
                }
            } else if (refName == stateTypeRef()) {
                QPair<bool, QString> result = validateMap(stateTypeDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "State type not matching";
                    return result;
                }
            } else if (refName == stateEvaluatorRef()) {
                QPair<bool, QString> result = validateMap(stateEvaluatorDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "StateEvaluator type not matching";
                    return result;
                }
            } else if (refName == stateDescriptorRef()) {
                QPair<bool, QString> result = validateMap(stateDescriptorDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "StateDescriptor type not matching";
                    return result;
                }
            } else if (refName == pluginRef()) {
                QPair<bool, QString> result = validateMap(pluginDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Plugin not matching";
                    return result;
                }
            } else if (refName == ruleRef()) {
                QPair<bool, QString> result = validateMap(ruleDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Rule type not matching";
                    return result;
                }
            } else if (refName == ruleDescriptionRef()) {
                QPair<bool, QString> result = validateMap(s_ruleDescription, variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "RuleDescription type not matching";
                    return result;
                }
            } else if (refName == stateRef()) {
                QPair<bool, QString> result = validateMap(s_state, variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "State not matching";
                    return result;
                }
            } else if (refName == eventDescriptorRef()) {
                QPair<bool, QString> result = validateMap(eventDescriptorDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "Eventdescriptor not matching";
                    return result;
                }
            } else if (refName == logEntryRef()) {
                QPair<bool, QString> result = validateMap(logEntryDescription(), variant.toMap());
                if (!result.first) {
                    qCWarning(dcJsonRpc) << "LogEntry not matching";
                    return result;
                }
            } else if (refName == basicTypeRef()) {
                QPair<bool, QString> result = validateBasicType(variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(basicTypeRef());
                    return result;
                }
            } else if (refName == stateOperatorRef()) {
                QPair<bool, QString> result = validateEnum(s_stateOperator, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(stateOperatorRef());
                    return result;
                }
            } else if (refName == createMethodRef()) {
                QPair<bool, QString> result = validateEnum(s_createMethod, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(createMethodRef());
                    return result;
                }
            } else if (refName == setupMethodRef()) {
                QPair<bool, QString> result = validateEnum(s_setupMethod, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(setupMethodRef());
                    return result;
                }
            } else if (refName == valueOperatorRef()) {
                QPair<bool, QString> result = validateEnum(s_valueOperator, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(valueOperatorRef());
                    return result;
                }
            } else if (refName == deviceErrorRef()) {
                QPair<bool, QString> result = validateEnum(s_deviceError, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(deviceErrorRef());
                    return result;
                }
            } else if (refName == authenticationErrorRef()) {
                QPair<bool, QString> result = validateEnum(s_authenticationError, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("value %1 not allowed in %2").arg(variant.toString()).arg(authenticationErrorRef());
                    return result;
                }
            } else if (refName == ruleErrorRef()) {
                QPair<bool, QString> result = validateEnum(s_ruleError, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(ruleErrorRef());
                    return result;
                }
            } else if (refName == loggingErrorRef()) {
                QPair<bool, QString> result = validateEnum(s_loggingError, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(loggingErrorRef());
                    return result;
                }
            } else if (refName == loggingSourceRef()) {
                QPair<bool, QString> result = validateEnum(s_loggingSource, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(loggingSourceRef());
                    return result;
                }
            } else if (refName == loggingLevelRef()) {
                QPair<bool, QString> result = validateEnum(s_loggingLevel, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(loggingLevelRef());
                    return result;
                }
            } else if (refName == loggingEventTypeRef()) {
                QPair<bool, QString> result = validateEnum(s_loggingEventType, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(loggingEventTypeRef());
                    return result;
                }
            } else if (refName == inputTypeRef()) {
                QPair<bool, QString> result = validateEnum(s_inputType, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(inputTypeRef());
                    return result;
                }
            } else if (refName == unitRef()) {
                QPair<bool, QString> result = validateEnum(s_unit, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(unitRef());
                    return result;
                }
            } else if (refName == basicTagRef()) {
                QPair<bool, QString> result = validateEnum(s_basicTag, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(basicTagRef());
                    return result;
                }
            } else if (refName == deviceIconRef()) {
                QPair<bool, QString> result = validateEnum(s_deviceIcon, variant);
                if (!result.first) {
                    qCWarning(dcJsonRpc) << QString("Value %1 not allowed in %2").arg(variant.toString()).arg(deviceIconRef());
                    return result;
                }
            } else {
                Q_ASSERT_X(false, "JsonTypes", QString("Unhandled ref: %1").arg(refName).toLatin1().data());
                return report(false, QString("Unhandled ref %1. Server implementation incomplete.").arg(refName));
            }

        } else {
            QPair<bool, QString> result = JsonTypes::validateProperty(templateVariant, variant);
            if (!result.first) {
                qCWarning(dcJsonRpc) << "property not matching:" << templateVariant << "!=" << variant;
                return result;
            }
        }
        break;
    case QVariant::Map: {
        QPair<bool, QString> result = validateMap(templateVariant.toMap(), variant.toMap());
        if (!result.first) {
            return result;
        }
        break;
    }
    case QVariant::List: {
        QPair<bool, QString> result = validateList(templateVariant.toList(), variant.toList());
        if (!result.first) {
            return result;
        }
        break;
    }
    default:
        qCWarning(dcJsonRpc) << "Unhandled value" << templateVariant;
        return report(false, QString("Unhandled value %1.").arg(templateVariant.toString()));
    }
    return report(true, "");
}

QPair<bool, QString> JsonTypes::validateBasicType(const QVariant &variant)
{
    if (variant.canConvert(QVariant::Uuid) && QVariant(variant).convert(QVariant::Uuid)) {
        return report(true, "");
    }
    if (variant.canConvert(QVariant::String) && QVariant(variant).convert(QVariant::String)) {
        return report(true, "");
    }
    if (variant.canConvert(QVariant::Int) && QVariant(variant).convert(QVariant::Int)) {
        return report(true, "");
    }
    if (variant.canConvert(QVariant::UInt) && QVariant(variant).convert(QVariant::UInt)){
        return report(true, "");
    }
    if (variant.canConvert(QVariant::Double) && QVariant(variant).convert(QVariant::Double)) {
        return report(true, "");
    }
    if (variant.canConvert(QVariant::Bool && QVariant(variant).convert(QVariant::Bool))) {
        return report(true, "");
    }
    if (variant.canConvert(QVariant::Color) && QVariant(variant).convert(QVariant::Color)) {
        return report(true, "");
    }
    return report(false, QString("Error validating basic type %1.").arg(variant.toString()));
}

QVariant::Type JsonTypes::getActionParamType(const ActionTypeId &actionTypeId, const QString &paramName)
{
    foreach (const DeviceClass &deviceClass, GuhCore::instance()->deviceManager()->supportedDevices()) {
        foreach (const ActionType &actionType, deviceClass.actionTypes()) {
            if (actionType.id() == actionTypeId) {
                foreach (const ParamType &paramType, actionType.paramTypes()) {
                    if (paramType.name() == paramName) {
                        return paramType.type();
                    }
                }
            }
        }
    }
    return QVariant::Invalid;
}

QVariant::Type JsonTypes::getEventParamType(const EventTypeId &eventTypeId, const QString &paramName)
{
    foreach (const DeviceClass &deviceClass, GuhCore::instance()->deviceManager()->supportedDevices()) {
        foreach (const EventType &eventType, deviceClass.eventTypes()) {
            if (eventType.id() == eventTypeId) {
                foreach (const ParamType &paramType, eventType.paramTypes()) {
                    // get ParamType of Event
                    if (paramType.name() == paramName) {
                        return paramType.type();
                    }
                }
            }
        }
    }
    return QVariant::Invalid;
}

bool JsonTypes::checkEventDescriptors(const QList<EventDescriptor> eventDescriptors, const EventTypeId &eventTypeId)
{
    foreach (const EventDescriptor eventDescriptor, eventDescriptors) {
        if (eventDescriptor.eventTypeId() == eventTypeId) {
            return true;
        }
    }
    return false;
}

RuleEngine::RuleError JsonTypes::verifyRuleConsistency(const QVariantMap &params)
{
    // check if there are an eventDescriptor and an eventDescriptorList
    if (params.contains("eventDescriptor") && params.contains("eventDescriptorList")) {
        qCWarning(dcJsonRpc) << "Only one of eventDesciptor or eventDescriptorList may be used.";
        return RuleEngine::RuleErrorInvalidParameter;
    }

    // check if this rules is based on any event and contains exit actions
    if (params.contains("eventDescriptor") || params.contains("eventDescriptorList")) {
        if (params.contains("exitActions")) {
            qCWarning(dcJsonRpc) << "The exitActions will never be executed if the rule contains an eventDescriptor.";
            return RuleEngine::RuleErrorInvalidRuleFormat;
        }
    }

    // check if there are any actions
    if (params.value("actions").toList().isEmpty()) {
        qCWarning(dcJsonRpc) << "Rule actions missing. A rule without actions has no effect.";
        return RuleEngine::RuleErrorMissingParameter;
    }

    // TODO: check if events and stateEvaluators are missing

    return RuleEngine::RuleErrorNoError;
}

QPair<QList<EventDescriptor>, RuleEngine::RuleError> JsonTypes::verifyEventDescriptors(const QVariantMap &params)
{
    // Check and unpack eventDescriptors
    QList<EventDescriptor> eventDescriptorList = QList<EventDescriptor>();
    if (params.contains("eventDescriptor")) {
        QVariantMap eventMap = params.value("eventDescriptor").toMap();
        qCDebug(dcJsonRpc) << "unpacking eventDescriptor" << eventMap;
        eventDescriptorList.append(JsonTypes::unpackEventDescriptor(eventMap));
    } else if (params.contains("eventDescriptorList")) {
        QVariantList eventDescriptors = params.value("eventDescriptorList").toList();
        qCDebug(dcJsonRpc) << "unpacking eventDescriptorList:" << eventDescriptors;
        foreach (const QVariant &eventVariant, eventDescriptors) {
            QVariantMap eventMap = eventVariant.toMap();
            eventDescriptorList.append(JsonTypes::unpackEventDescriptor(eventMap));
        }
    }
    return QPair<QList<EventDescriptor>, RuleEngine::RuleError>(eventDescriptorList, RuleEngine::RuleErrorNoError);
}

QPair<QList<RuleAction>, RuleEngine::RuleError> JsonTypes::verifyActions(const QVariantMap &params, const QList<EventDescriptor> &eventDescriptorList)
{
    QList<RuleAction> actions;
    QVariantList actionList = params.value("actions").toList();
    if (actionList.isEmpty()) {
        qCWarning(dcJsonRpc) << "Rule has no actions. This rule will do nothing.";
        return QPair<QList<RuleAction>, RuleEngine::RuleError>(actions, RuleEngine::RuleErrorInvalidRuleFormat);
    }

    qCDebug(dcJsonRpc) << "unpacking actions:" << actionList;
    foreach (const QVariant &actionVariant, actionList) {
        QVariantMap actionMap = actionVariant.toMap();
        RuleAction action(ActionTypeId(actionMap.value("actionTypeId").toString()), DeviceId(actionMap.value("deviceId").toString()));
        RuleActionParamList actionParamList = JsonTypes::unpackRuleActionParams(actionMap.value("ruleActionParams").toList());
        foreach (const RuleActionParam &ruleActionParam, actionParamList) {
            if (!ruleActionParam.isValid()) {
                qCWarning(dcJsonRpc) << "got an actionParam with value AND eventTypeId!";
                return QPair<QList<RuleAction>, RuleEngine::RuleError>(actions, RuleEngine::RuleErrorInvalidRuleActionParameter);
            }
        }
        qCDebug(dcJsonRpc) << "params in exitAction" << action.ruleActionParams();
        action.setRuleActionParams(actionParamList);
        actions.append(action);
    }

    // check possible eventTypeIds in params
    foreach (const RuleAction &ruleAction, actions) {
        if (ruleAction.isEventBased()) {
            foreach (const RuleActionParam &ruleActionParam, ruleAction.ruleActionParams()) {
                if (ruleActionParam.eventTypeId() != EventTypeId()) {
                    // We have an eventTypeId
                    if (eventDescriptorList.isEmpty()) {
                        qCWarning(dcJsonRpc) << "RuleAction" << ruleAction.actionTypeId() << "contains an eventTypeId, but there are no eventDescriptors.";
                        return QPair<QList<RuleAction>, RuleEngine::RuleError>(actions, RuleEngine::RuleErrorInvalidRuleActionParameter);
                    }
                    // now check if this eventType is in the eventDescriptorList of this rule
                    if (!checkEventDescriptors(eventDescriptorList, ruleActionParam.eventTypeId())) {
                        qCWarning(dcJsonRpc) << "eventTypeId from RuleAction" << ruleAction.actionTypeId() << "missing in eventDescriptors.";
                        return QPair<QList<RuleAction>, RuleEngine::RuleError>(actions, RuleEngine::RuleErrorInvalidRuleActionParameter);
                    }

                    // check if the param type of the event and the action match
                    QVariant::Type eventParamType = getEventParamType(ruleActionParam.eventTypeId(), ruleActionParam.eventParamName());
                    QVariant::Type actionParamType = getActionParamType(ruleAction.actionTypeId(), ruleActionParam.name());
                    if (eventParamType != actionParamType) {
                        qCWarning(dcJsonRpc) << "RuleActionParam" << ruleActionParam.name() << " and given event param " << ruleActionParam.eventParamName() << "have not the same type:";
                        qCWarning(dcJsonRpc) << "        -> actionParamType:" << actionParamType;
                        qCWarning(dcJsonRpc) << "        ->  eventParamType:" << eventParamType;
                        return QPair<QList<RuleAction>, RuleEngine::RuleError>(actions, RuleEngine::RuleErrorTypesNotMatching);
                    }
                }
            }
        }
    }
    return QPair<QList<RuleAction>, RuleEngine::RuleError>(actions, RuleEngine::RuleErrorNoError);
}

QPair<QList<RuleAction>, RuleEngine::RuleError> JsonTypes::verifyExitActions(const QVariantMap &params)
{
    QList<RuleAction> exitActions;
    if (params.contains("exitActions")) {
        QVariantList exitActionList = params.value("exitActions").toList();
        qCDebug(dcJsonRpc) << "unpacking exitActions:" << exitActionList;
        foreach (const QVariant &actionVariant, exitActionList) {
            QVariantMap actionMap = actionVariant.toMap();
            RuleAction action(ActionTypeId(actionMap.value("actionTypeId").toString()), DeviceId(actionMap.value("deviceId").toString()));
            if (action.isEventBased()) {
                qCWarning(dcJsonRpc) << "got exitAction with a param value containing an eventTypeId!";
                return QPair<QList<RuleAction>, RuleEngine::RuleError>(exitActions, RuleEngine::RuleErrorInvalidRuleActionParameter);
            }
            qCDebug(dcJsonRpc) << "params in exitAction" << action.ruleActionParams();
            action.setRuleActionParams(JsonTypes::unpackRuleActionParams(actionMap.value("ruleActionParams").toList()));
            exitActions.append(action);
        }
    }
    return QPair<QList<RuleAction>, RuleEngine::RuleError>(exitActions, RuleEngine::RuleErrorNoError);
}

QPair<bool, QString> JsonTypes::validateEnum(const QVariantList &enumDescription, const QVariant &value)
{
    QStringList enumStrings;
    foreach (const QVariant &variant, enumDescription) {
        enumStrings.append(variant.toString());
    }

    return report(enumDescription.contains(value.toString()), QString("Value %1 not allowed in %2").arg(value.toString()).arg(enumStrings.join(", ")));
}

}
