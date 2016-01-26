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

#ifndef GUHTESTBASE_H
#define GUHTESTBASE_H

#include "typeutils.h"
#include "logging/logging.h"
#include "mocktcpserver.h"
#include "devicemanager.h"
#include "ruleengine.h"
#include "jsontypes.h"
#include "authenticationmanager.h"

#include <QObject>
#include <QUuid>
#include <QVariantMap>
#include <QSignalSpy>
#include <QtTest>

extern DeviceClassId mockDeviceClassId;
extern DeviceClassId mockDeviceAutoClassId;
extern DeviceClassId mockPushButtonDeviceClassId;
extern DeviceClassId mockDisplayPinDeviceClassId;
extern DeviceClassId mockParentDeviceClassId;
extern DeviceClassId mockChildDeviceClassId;
extern DeviceClassId mockDeviceDiscoveryClassId;
extern DeviceClassId mockDeviceAsyncSetupClassId;
extern DeviceClassId mockDeviceBrokenClassId;
extern DeviceClassId mockDeviceBrokenAsyncSetupClassId;
extern ActionTypeId mockActionIdWithParams;
extern ActionTypeId mockActionIdNoParams;
extern ActionTypeId mockActionIdAsync;
extern ActionTypeId mockActionIdFailing;
extern ActionTypeId mockActionIdAsyncFailing;
extern EventTypeId mockEvent1Id;
extern EventTypeId mockEvent2Id;
extern StateTypeId mockIntStateId;
extern StateTypeId mockBoolStateId;

extern QString testToken;
extern QString testUserName;
extern QString testUserPassword;
extern UserId testUserId;

using namespace guhserver;

class MockTcpServer;

class GuhTestBase : public QObject
{
    Q_OBJECT
public:
    explicit GuhTestBase(QObject *parent = 0);

protected slots:
    void initTestCase();
    void cleanupTestCase();

protected:
    QVariant injectAndWait(const QString &method, const QVariantMap &params = QVariantMap(), const QString &token = "4VzMAR3PozoPKkwCb0x0-pBWESwdL5YdRGbJn4I9TRE");
    QVariant checkNotification(const QSignalSpy &spy, const QString &notification);

    QVariant getAndWait(const QNetworkRequest &request, const int &expectedStatus = 200);
    QVariant deleteAndWait(const QNetworkRequest &request, const int &expectedStatus = 200);
    QVariant postAndWait(const QNetworkRequest &request, const QVariant &params, const int &expectedStatus = 200);
    QVariant putAndWait(const QNetworkRequest &request, const QVariant &params, const int &expectedStatus = 200);

    void verifyReply(QNetworkReply *reply, const QByteArray &data, const int &expectedStatus);

    bool enableNotifications();
    bool disableNotifications();

    inline void verifyError(const QVariant &response, const QString &fieldName, const QString &error)
    {
        QJsonDocument jsonDoc = QJsonDocument::fromVariant(response);
        QVERIFY2(response.toMap().value("status").toString() == QString("success"),
                 QString("\nExpected status: \"success\"\nGot: %2\nFull message: %3")
                 .arg(response.toMap().value("status").toString())
                 .arg(jsonDoc.toJson().data())
                 .toLatin1().data());
        QVERIFY2(response.toMap().value("params").toMap().value(fieldName).toString() == error,
                 QString("\nExpected: %1\nGot: %2\nFull message: %3\n")
                 .arg(error)
                 .arg(response.toMap().value("params").toMap().value(fieldName).toString())
                 .arg(jsonDoc.toJson().data())
                 .toLatin1().data());
    }

    inline void verifyRuleError(const QVariant &response, RuleEngine::RuleError error = RuleEngine::RuleErrorNoError) {
        verifyError(response, "ruleError", JsonTypes::ruleErrorToString(error));
    }

    inline void verifyDeviceError(const QVariant &response, DeviceManager::DeviceError error = DeviceManager::DeviceErrorNoError) {
        verifyError(response, "deviceError", JsonTypes::deviceErrorToString(error));
    }

    inline void verifyLoggingError(const QVariant &response, Logging::LoggingError error = Logging::LoggingErrorNoError) {
        verifyError(response, "loggingError", JsonTypes::loggingErrorToString(error));
    }

    inline void verifyAuthenticationError(const QVariant &response, AuthenticationManager::AuthenticationError error = AuthenticationManager::AuthenticationErrorNoError) {
        verifyError(response, "authenticationError", JsonTypes::authenticationErrorToString(error));
    }

    inline void verifyParams(const QVariantList &requestList, const QVariantList &responseList, bool allRequired = true)
    {
        if (allRequired)
            QVERIFY2(requestList.count() == responseList.count(), "Not the same count of param in response.");
        foreach (const QVariant &requestParam, requestList) {
            bool found = false;
            foreach (const QVariant &responseParam, responseList) {
                if (requestParam.toMap().value("name") == responseParam.toMap().value("name")){
                    QCOMPARE(requestParam.toMap().value("value"), responseParam.toMap().value("value"));
                    found = true;
                    break;
                }
            }
            if (allRequired)
                QVERIFY2(found, "Param missing");
        }
    }

    // just for debugging
    inline void printJson(const QVariant &response) {
        QJsonDocument jsonDoc = QJsonDocument::fromVariant(response);
        qDebug() << jsonDoc.toJson();
    }

    void restartServer();
    void clearLoggingDatabase();

protected:
    PluginId mockPluginId = PluginId("727a4a9a-c187-446f-aadf-f1b2220607d1");
    VendorId guhVendorId = VendorId("2062d64d-3232-433c-88bc-0d33c0ba2ba6");

    MockTcpServer *m_mockTcpServer;
    QUuid m_clientId;
    int m_commandId;

    int m_mockDevice1Port;
    int m_mockDevice2Port;

    DeviceId m_mockDeviceId;

};

#endif // GUHTESTBASE_H
