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

#ifndef JSONTYPES_H
#define JSONTYPES_H

#include "plugin/deviceclass.h"
#include "plugin/devicedescriptor.h"
#include "rule.h"
#include "devicemanager.h"
#include "ruleengine.h"
#include "authenticationmanager.h"

#include "types/event.h"
#include "types/action.h"
#include "types/actiontype.h"
#include "types/paramtype.h"
#include "types/paramdescriptor.h"
#include "types/ruleactionparam.h"

#include "logging/logging.h"
#include "logging/logentry.h"
#include "logging/logfilter.h"

#include <QObject>

#include <QVariantMap>
#include <QString>
#include <QMetaEnum>

class DevicePlugin;
class Device;

namespace guhserver {

#define DECLARE_OBJECT(typeName, jsonName) \
    public: \
    static QString typeName##Ref() { return QStringLiteral("$ref:") + QStringLiteral(jsonName); } \
    static QVariantMap typeName##Description() { \
        if (!s_initialized) { init(); } \
        return s_##typeName; \
    } \
    private: \
    static QVariantMap s_##typeName; \
    public:

#define DECLARE_TYPE(typeName, enumString, className, enumName) \
    public: \
    static QString typeName##Ref() { return QStringLiteral("$ref:") + QStringLiteral(enumString); } \
    static QVariantList typeName() { \
        if (!s_initialized) { init(); } \
        return s_##typeName; \
    } \
    static QString typeName##ToString(className::enumName value) { \
        QMetaObject metaObject = className::staticMetaObject; \
        int enumIndex = metaObject.indexOfEnumerator(enumString); \
        QMetaEnum metaEnum = metaObject.enumerator(enumIndex); \
        return metaEnum.valueToKey(metaEnum.value(value)); \
    } \
    private: \
    static QVariantList s_##typeName; \
    public:

class JsonTypes
{
    Q_GADGET
    Q_ENUMS(BasicType)
    Q_ENUMS(JsonError)

public:
    enum BasicType {
        Uuid,
        String,
        Int,
        Uint,
        Double,
        Bool,
        Variant,
        Color,
        Object
    };

    static QVariantMap allTypes();

    DECLARE_TYPE(basicType, "BasicType", JsonTypes, BasicType)
    DECLARE_TYPE(basicTag, "BasicTag", DeviceClass, BasicTag)
    DECLARE_TYPE(stateOperator, "StateOperator", Types, StateOperator)
    DECLARE_TYPE(valueOperator, "ValueOperator", Types, ValueOperator)
    DECLARE_TYPE(inputType, "InputType", Types, InputType)
    DECLARE_TYPE(unit, "Unit", Types, Unit)
    DECLARE_TYPE(createMethod, "CreateMethod", DeviceClass, CreateMethod)
    DECLARE_TYPE(setupMethod, "SetupMethod", DeviceClass, SetupMethod)
    DECLARE_TYPE(deviceError, "DeviceError", DeviceManager, DeviceError)
    DECLARE_TYPE(removePolicy, "RemovePolicy", RuleEngine, RemovePolicy)
    DECLARE_TYPE(ruleError, "RuleError", RuleEngine, RuleError)
    DECLARE_TYPE(loggingError, "LoggingError", Logging, LoggingError)
    DECLARE_TYPE(loggingSource, "LoggingSource", Logging, LoggingSource)
    DECLARE_TYPE(loggingLevel, "LoggingLevel", Logging, LoggingLevel)
    DECLARE_TYPE(loggingEventType, "LoggingEventType", Logging, LoggingEventType)
    DECLARE_TYPE(authenticationError, "AuthenticationError", AuthenticationManager, AuthenticationError)

    DECLARE_OBJECT(paramType, "ParamType")
    DECLARE_OBJECT(param, "Param")
    DECLARE_OBJECT(paramDescriptor, "ParamDescriptor")
    DECLARE_OBJECT(ruleAction, "RuleAction")
    DECLARE_OBJECT(ruleActionParam, "RuleActionParam")
    DECLARE_OBJECT(stateType, "StateType")
    DECLARE_OBJECT(stateDescriptor, "StateDescriptor")
    DECLARE_OBJECT(state, "State")
    DECLARE_OBJECT(stateEvaluator, "StateEvaluator")
    DECLARE_OBJECT(eventType, "EventType")
    DECLARE_OBJECT(event, "Event")
    DECLARE_OBJECT(eventDescriptor, "EventDescriptor")
    DECLARE_OBJECT(actionType, "ActionType")
    DECLARE_OBJECT(action, "Action")
    DECLARE_OBJECT(plugin, "Plugin")
    DECLARE_OBJECT(vendor, "Vendor")
    DECLARE_OBJECT(deviceClass, "DeviceClass")
    DECLARE_OBJECT(device, "Device")
    DECLARE_OBJECT(deviceDescriptor, "DeviceDescriptor")
    DECLARE_OBJECT(rule, "Rule")
    DECLARE_OBJECT(ruleDescription, "RuleDescription")
    DECLARE_OBJECT(logEntry, "LogEntry")

    // pack types    
    static QVariantMap packEventType(const EventType &eventType);
    static QVariantMap packEvent(const Event &event);
    static QVariantMap packEventDescriptor(const EventDescriptor &event);
    static QVariantMap packActionType(const ActionType &actionType);
    static QVariantMap packAction(const Action &action);
    static QVariantMap packRuleAction(const RuleAction &ruleAction);
    static QVariantMap packRuleActionParam(const RuleActionParam &ruleActionParam);
    static QVariantMap packState(const State &state);
    static QVariantMap packStateType(const StateType &stateType);
    static QVariantMap packStateDescriptor(const StateDescriptor &stateDescriptor);
    static QVariantMap packStateEvaluator(const StateEvaluator &stateEvaluator);
    static QVariantMap packParam(const Param &param);
    static QVariantMap packParamType(const ParamType &paramType);
    static QVariantMap packParamDescriptor(const ParamDescriptor &paramDescriptor);
    static QVariantMap packVendor(const Vendor &vendor);
    static QVariantMap packDeviceClass(const DeviceClass &deviceClass);
    static QVariantMap packPlugin(DevicePlugin *plugin);
    static QVariantMap packDevice(Device *device);
    static QVariantMap packDeviceDescriptor(const DeviceDescriptor &descriptor);
    static QVariantMap packRule(const Rule &rule);
    static QVariantList packRules(const QList<Rule> rules);
    static QVariantMap packRuleDescription(const Rule &rule);
    static QVariantMap packLogEntry(const LogEntry &logEntry);
    static QVariantList packCreateMethods(DeviceClass::CreateMethods createMethods);

    // pack resources
    static QVariantList packSupportedVendors();
    static QVariantList packSupportedDevices(const VendorId &vendorId);
    static QVariantList packConfiguredDevices();
    static QVariantList packDeviceStates(Device *device);
    static QVariantList packDeviceDescriptors(const QList<DeviceDescriptor> deviceDescriptors);

    static QVariantList packRuleDescriptions();
    static QVariantList packRuleDescriptions(const QList<Rule> &rules);

    static QVariantList packActionTypes(const DeviceClass &deviceClass);
    static QVariantList packStateTypes(const DeviceClass &deviceClass);
    static QVariantList packEventTypes(const DeviceClass &deviceClass);
    static QVariantList packPlugins();

    static QString basicTypeToString(const QVariant::Type &type);

    // unpack Types
    static Param unpackParam(const QVariantMap &paramMap);
    static ParamList unpackParams(const QVariantList &paramList);
    static RuleActionParam unpackRuleActionParam(const QVariantMap &ruleActionParamMap);
    static RuleActionParamList unpackRuleActionParams(const QVariantList &ruleActionParamList);
    static ParamDescriptor unpackParamDescriptor(const QVariantMap &paramDescriptorMap);
    static QList<ParamDescriptor> unpackParamDescriptors(const QVariantList &paramDescriptorList);
    static EventDescriptor unpackEventDescriptor(const QVariantMap &eventDescriptorMap);
    static StateEvaluator unpackStateEvaluator(const QVariantMap &stateEvaluatorMap);
    static StateDescriptor unpackStateDescriptor(const QVariantMap &stateDescriptorMap);
    static LogFilter unpackLogFilter(const QVariantMap &logFilterMap);

    // validate
    static QPair<bool, QString> validateMap(const QVariantMap &templateMap, const QVariantMap &map);
    static QPair<bool, QString> validateProperty(const QVariant &templateValue, const QVariant &value);
    static QPair<bool, QString> validateList(const QVariantList &templateList, const QVariantList &list);
    static QPair<bool, QString> validateVariant(const QVariant &templateVariant, const QVariant &variant);
    static QPair<bool, QString> validateEnum(const QVariantList &enumList, const QVariant &value);
    static QPair<bool, QString> validateBasicType(const QVariant &variant);

    // rule validation helper methods
    static bool checkEventDescriptors(const QList<EventDescriptor> eventDescriptors, const EventTypeId &eventTypeId);
    static QVariant::Type getActionParamType(const ActionTypeId &actionTypeId, const QString &paramName);
    static QVariant::Type getEventParamType(const EventTypeId &eventTypeId, const QString &paramName);
    static RuleEngine::RuleError verifyRuleConsistency(const QVariantMap &params);
    static QPair<QList<EventDescriptor>, RuleEngine::RuleError> verifyEventDescriptors(const QVariantMap &params);
    static QPair<QList<RuleAction>, RuleEngine::RuleError> verifyActions(const QVariantMap &params, const QList<EventDescriptor> &eventDescriptorList);
    static QPair<QList<RuleAction>, RuleEngine::RuleError> verifyExitActions(const QVariantMap &params);

private:
    static bool s_initialized;
    static void init();

    static QPair<bool, QString> report(bool status, const QString &message);
    static QVariantList enumToStrings(const QMetaObject &metaObject, const QString &enumName);

    static QString s_lastError;
};

}

#endif // JSONTYPES_H
