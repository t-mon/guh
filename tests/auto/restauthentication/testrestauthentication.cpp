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

#include "guhtestbase.h"
#include "guhcore.h"
#include "devicemanager.h"
#include "mocktcpserver.h"
#include "webserver.h"

#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QHttpPart>
#include <QMetaType>

using namespace guhserver;

class TestRestAuthentication: public GuhTestBase
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

void TestRestAuthentication::testAuthenticate_data()
{
    QTest::addColumn<QString>("userName");
    QTest::addColumn<QString>("password");
    QTest::addColumn<int>("expectedStatusCode");

    QTest::newRow("Valid authentication") << "guh-test" << "guh_test_password@1234" << 200;
    QTest::newRow("Invalid authentication: wrong password") << "guh-test" << "wrong password!!@1234" << 401;
    QTest::newRow("Invalid authentication: wrong user") << "guh-tests" << "guh_test_password@1234" << 401;
    QTest::newRow("Invalid authentication: wrong user and wrong password") << "guh-tests" << "guh_test_password@1235" << 401;
}

void TestRestAuthentication::testAuthenticate()
{
    QFETCH(QString, userName);
    QFETCH(QString, password);
    QFETCH(int, expectedStatusCode);

    QNetworkRequest request(QUrl(QString("http://localhost:3333/api/v1/authentication/login")));
    QVariant response = getAndWait(request, expectedStatusCode, userName, password);

    if (expectedStatusCode == 200) {
        QVariantMap responseParams = response.toMap();
        QVERIFY(responseParams.contains("token"));
        QString token = responseParams.value("token").toString();
        qDebug() << "Got token" << token;

        // test new token
        QNetworkRequest newRequest(QUrl(QString("http://localhost:3333/api/v1/devices")));
        response = getAndWait(newRequest, 200, token);

        // check if the token is still allowed after restart
        restartServer();
        response = getAndWait(newRequest, 200, token);
    }
}

void TestRestAuthentication::testAccessTokens_data()
{
    QTest::addColumn<QString>("token");
    QTest::addColumn<int>("expectedStatusCode");

    QTest::newRow("Valid token") << testToken << 200;
    QTest::newRow("Invalid token") << "invalidtoken1kjb1h32kjhv12jhv3" << 401;
    QTest::newRow("Invalid token") << "manamana-diediediediedie_manamana" << 401;
    QTest::newRow("Invalid token") << QString() << 401;
}

void TestRestAuthentication::testAccessTokens()
{
    QFETCH(QString, token);
    QFETCH(int, expectedStatusCode);

    QNetworkRequest request(QUrl(QString("http://localhost:3333/api/v1/devices")));
    QVariant response = getAndWait(request, expectedStatusCode, token);

    if (expectedStatusCode != 200)
        QVERIFY(response.toMap().value("error").toString() == "AuthenticationErrorAuthenicationFailed");

}

void TestRestAuthentication::changePassword_data()
{
    QTest::addColumn<QString>("userName");
    QTest::addColumn<QString>("password");
    QTest::addColumn<QString>("newPassword");
    QTest::addColumn<int>("expectedStatusCode");
    QTest::addColumn<AuthenticationManager::AuthenticationError>("error");

    QTest::newRow("Change password successfully") << testUserName << testUserPassword << "!!@12Bla.$$||" << 200 << AuthenticationManager::AuthenticationErrorNoError;
    QTest::newRow("Change password back") << testUserName << "!!@12Bla.$$||" << testUserPassword << 200 << AuthenticationManager::AuthenticationErrorNoError;
    QTest::newRow("Invalid username") << "blub blub" << testUserPassword << testUserPassword << 401 << AuthenticationManager::AuthenticationErrorAuthenicationFailed;
    QTest::newRow("Invalid password") << testUserName << "blub blub" << "something_else" << 401 << AuthenticationManager::AuthenticationErrorAuthenicationFailed;
    QTest::newRow("Weak password - empty") << testUserName << testUserPassword << QString() << 400 << AuthenticationManager::AuthenticationErrorWeakPassword;
    QTest::newRow("Weak password - to short") << testUserName << testUserPassword << "1234" << 400 << AuthenticationManager::AuthenticationErrorWeakPassword;
}

void TestRestAuthentication::changePassword()
{
    QFETCH(QString, userName);
    QFETCH(QString, password);
    QFETCH(QString, newPassword);
    QFETCH(int, expectedStatusCode);
    QFETCH(AuthenticationManager::AuthenticationError, error);

    QVariantMap params;
    params.insert("userName", userName);
    params.insert("password", password);
    params.insert("newPassword", newPassword);

    QNetworkRequest request(QUrl(QString("http://localhost:3333/api/v1/authentication/changepassword")));
    QVariant response = putAndWait(request, params, expectedStatusCode);

    if (expectedStatusCode != 200)
        QVERIFY(response.toMap().value("error").toString() == JsonTypes::authenticationErrorToString(error));

}

void TestRestAuthentication::removeAuthorizedConnections()
{
    // authorize some times
    for (int i = 0; i < 10; i++) {
        QNetworkRequest request(QUrl(QString("http://localhost:3333/api/v1/authentication/login")));
        QVariant response = getAndWait(request, 200, testUserName, testUserPassword);
        QVERIFY(response.toMap().contains("token"));
    }

    // get authorized connections
    QStringList tokenList;
    QNetworkRequest request(QUrl(QString("http://localhost:3333/api/v1/authentication/authorizedconnections")));
    QVariant response = getAndWait(request);
    foreach (const QVariant connectionVariant, response.toList()) {
        QVariantMap connection = connectionVariant.toMap();
        QString token = connection.value("token").toString();
        if (token != testToken) {
            tokenList.append(token);
        }
    }

    response.clear();
    QVariantMap params;
    params.insert("tokens", tokenList);
    qDebug() << QJsonDocument::fromVariant(params).toJson();
    request = QNetworkRequest(QUrl(QString("http://localhost:3333/api/v1/authentication/removeconnections")));
    response = postAndWait(request, params);

    response.clear();
    request = QNetworkRequest(QUrl(QString("http://localhost:3333/api/v1/authentication/authorizedconnections")));
    response = getAndWait(request);
    QCOMPARE(response.toList().count(), 1);

    // verify that removed tokens not working any more
    foreach (const QVariant tokenVariant, tokenList) {
        QNetworkRequest request(QUrl(QString("http://localhost:3333/api/v1/devices")));
        QVariant response = getAndWait(request, 401, tokenVariant.toString());
        QVERIFY(response.toMap().value("error").toString() == "AuthenticationErrorAuthenicationFailed");
    }

    restartServer();

    response.clear();
    request = QNetworkRequest(QUrl(QString("http://localhost:3333/api/v1/authentication/authorizedconnections")));
    response = getAndWait(request);
    QCOMPARE(response.toList().count(), 1);
}

#include "testrestauthentication.moc"
QTEST_MAIN(TestRestAuthentication)
