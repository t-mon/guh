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

private:
    QString m_token;

};

void TestAuthentication::testAuthenticate_data()
{
    QTest::addColumn<QString>("userName");
    QTest::addColumn<QString>("password");
    QTest::addColumn<bool>("success");

    QTest::newRow("Valid authentication") << "guh-test" << "guh_test_password@1234" << true;
    QTest::newRow("Invalid authentication: wrong password") << "guh-test" << "wrong password!!@1234" << false;
    QTest::newRow("Invalid authentication: wrong user") << "guh-tests" << "guh_test_password@1234" << false;
    QTest::newRow("Invalid authentication: wrong user and wrong password") << "guh-tests" << "guh_test_password@1235" << false;
}

void TestAuthentication::testAuthenticate()
{
    QFETCH(QString, userName);
    QFETCH(QString, password);
    QFETCH(bool, success);

    QVariantMap params;
    params.insert("clientDescription", "guh-tests");
    params.insert("userName", userName);
    params.insert("password", password);

    QVariant response = injectAndWait("Authentication.Authenticate", params);
    QVERIFY(!response.isNull());

    QVariantMap responseParams = response.toMap().value("params").toMap();
    if (success) {
        QVERIFY(responseParams.contains("token"));
        m_token = params.value("token").toString();
        qDebug() << "Got token" << m_token;
        QVERIFY(responseParams.value("authenticationError").toString() == "AuthenticationErrorNoError");

        // test new token
        response = injectAndWait("JSONRPC.Introspect", QVariantMap(), m_token);
        QVERIFY(!response.isNull());

        restartServer();

        response = injectAndWait("JSONRPC.Introspect", QVariantMap(), m_token);


    } else {
        QVERIFY(responseParams.value("authenticationError").toString() == "AuthenticationErrorAuthenicationFailed");
        qDebug() << QJsonDocument::fromVariant(response).toJson();
    }
}

#include "testauthentication.moc"
QTEST_MAIN(TestAuthentication)
