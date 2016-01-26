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

#include "guhtestbase.h"
#include "guhcore.h"
#include "devicemanager.h"
#include "mocktcpserver.h"
#include "authenticationmanager.h"

#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QMetaType>

using namespace guhserver;

class TestAuthentication: public GuhTestBase
{
    Q_OBJECT

private slots:

    void testAuthenticate_data();
    void testAuthenticate();

    void testAccessTokens_data();
    void testAccessTokens();

    void changePassword_data();
    void changePassword();

    void removeAuthorizedConnections();

};

void TestAuthentication::testAuthenticate_data()
{
    QTest::addColumn<QString>("userName");
    QTest::addColumn<QString>("password");
    QTest::addColumn<AuthenticationManager::AuthenticationError>("error");

    QTest::newRow("Valid authentication") << "guh-test" << "guh_test_password@1234" << AuthenticationManager::AuthenticationErrorNoError;
    QTest::newRow("Invalid authentication: wrong password") << "guh-test" << "wrong password!!@1234" << AuthenticationManager::AuthenticationErrorAuthenicationFailed;
    QTest::newRow("Invalid authentication: wrong user") << "guh-tests" << "guh_test_password@1234" << AuthenticationManager::AuthenticationErrorAuthenicationFailed;
    QTest::newRow("Invalid authentication: wrong user and wrong password") << "guh-tests" << "guh_test_password@1235" << AuthenticationManager::AuthenticationErrorAuthenicationFailed;
}

void TestAuthentication::testAuthenticate()
{
    QFETCH(QString, userName);
    QFETCH(QString, password);
    QFETCH(AuthenticationManager::AuthenticationError, error);

    QVariantMap params;
    params.insert("clientDescription", "guh-tests");
    params.insert("userName", userName);
    params.insert("password", password);

    QVariant response = injectAndWait("Authentication.Authenticate", params);
    verifyAuthenticationError(response, error);

    if (error == AuthenticationManager::AuthenticationErrorNoError) {
        QVariantMap responseParams = response.toMap().value("params").toMap();
        QVERIFY(responseParams.contains("token"));
        QString token = responseParams.value("token").toString();
        qDebug() << "Got token" << token;
        QVERIFY(responseParams.value("authenticationError").toString() == "AuthenticationErrorNoError");

        // test new token
        response = injectAndWait("JSONRPC.Introspect", QVariantMap(), token);
        QVERIFY(response.toMap().value("status").toString() == "success");

        // check if the token is still allowed after restart
        restartServer();
        response = injectAndWait("JSONRPC.Introspect", QVariantMap(), token);
        QVERIFY(response.toMap().value("status").toString() == "success");
    }
}

void TestAuthentication::testAccessTokens_data()
{
    QTest::addColumn<QString>("token");
    QTest::addColumn<bool>("success");

    QTest::newRow("Valid token") << testToken << true;
    QTest::newRow("Invalid token") << "invalidtoken1kjb1h32kjhv12jhv3" << false;
    QTest::newRow("Invalid token") << "manamana-diediediediedie_manamana" << false;
    QTest::newRow("Invalid token") << QString() << false;
}

void TestAuthentication::testAccessTokens()
{
    QFETCH(QString, token);
    QFETCH(bool, success);

    QVariant response = injectAndWait("JSONRPC.Introspect", QVariantMap(), token);
    QVERIFY(!response.isNull());

    if (!success) {
        QVERIFY(response.toMap().value("status").toString() == "error");
        QVERIFY(response.toMap().value("error").toString() == "Authentication failed");
    } else {
        QVERIFY(response.toMap().value("status").toString() == "success");
    }
}

void TestAuthentication::changePassword_data()
{
    QTest::addColumn<QString>("userName");
    QTest::addColumn<QString>("password");
    QTest::addColumn<QString>("newPassword");
    QTest::addColumn<AuthenticationManager::AuthenticationError>("error");

    QTest::newRow("Change password successfully") << testUserName << testUserPassword << "!!@12Bla.$$||" << AuthenticationManager::AuthenticationErrorNoError;
    QTest::newRow("Change password back") << testUserName << "!!@12Bla.$$||" << testUserPassword << AuthenticationManager::AuthenticationErrorNoError;
    QTest::newRow("Invalid username") << "blub blub" << testUserPassword << testUserPassword << AuthenticationManager::AuthenticationErrorAuthenicationFailed;
    QTest::newRow("Invalid password") << testUserName << "blub blub" << "something_else" << AuthenticationManager::AuthenticationErrorAuthenicationFailed;
    QTest::newRow("Weak password - empty") << testUserName << testUserPassword << QString() << AuthenticationManager::AuthenticationErrorWeakPassword;
    QTest::newRow("Weak password - to short") << testUserName << testUserPassword << "1234" << AuthenticationManager::AuthenticationErrorWeakPassword;
}

void TestAuthentication::changePassword()
{
    QFETCH(QString, userName);
    QFETCH(QString, password);
    QFETCH(QString, newPassword);
    QFETCH(AuthenticationManager::AuthenticationError, error);

    QVariantMap params;
    params.insert("userName", userName);
    params.insert("password", password);
    params.insert("newPassword", newPassword);

    QVariant response = injectAndWait("Authentication.ChangePassword", params);
    verifyAuthenticationError(response, error);
}

void TestAuthentication::removeAuthorizedConnections()
{
    // authorize some times
    QVariantMap params;
    params.insert("clientDescription", "guh-tests");
    params.insert("userName", testUserName);
    params.insert("password", testUserPassword);

    for (int i = 0; i < 10; i++) {
        QVariant response = injectAndWait("Authentication.Authenticate", params);
        verifyAuthenticationError(response);
    }

    // get authorized connections
    QVariant response = injectAndWait("Authentication.GetAuthorizedConnections");
    verifyAuthenticationError(response);

    QVariantList tokenList;
    foreach (const QVariant connectionVariant, response.toMap().value("params").toMap().value("connections").toList()) {
        QVariantMap connection = connectionVariant.toMap();
        QVariant token = connection.value("token");
        qDebug() << token;
        if (token.toString() != testToken)
            tokenList.append(token);

    }

    params.clear(); response.clear();
    params.insert("tokens", tokenList);
    response = injectAndWait("Authentication.RemoveAuthorizedConnections", params);
    verifyAuthenticationError(response);

    // check if only one connection left
    response = injectAndWait("Authentication.GetAuthorizedConnections");
    verifyAuthenticationError(response);
    QCOMPARE(response.toMap().value("params").toMap().value("connections").toList().count(), 1);

    qDebug() << tokenList.count();

    // verify that removed tokens not working any more
    foreach (const QVariant tokenVariant, tokenList) {
        response = injectAndWait("JSONRPC.Introspect", QVariantMap(), tokenVariant.toString());
        QVERIFY(response.toMap().value("status").toString() == "error");
        QVERIFY(response.toMap().value("error").toString() == "Authentication failed");
    }

    restartServer();

    // check if only one connection left after restart
    response = injectAndWait("Authentication.GetAuthorizedConnections");
    verifyAuthenticationError(response);
    QCOMPARE(response.toMap().value("params").toMap().value("connections").toList().count(), 1);
}

#include "testauthentication.moc"
QTEST_MAIN(TestAuthentication)
