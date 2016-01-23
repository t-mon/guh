/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2014 Michael Zanetti <michael_zanetti@gmx.net>           *
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

/*!
    \class guhserver::LogEngine
    \brief  The engine which creates the log databse and provides access to it.

    \ingroup logs
    \inmodule core

    The \l{LogEngine} creates a \l{https://sqlite.org/}{SQLite3} database to stores everything what's
    happening in the system. The database can be accessed from the API's. To controll the size of the database the
    limit of the databse are 8000 entries.


    \sa LogEntry, LogFilter, LogsResource, LoggingHandler
*/

/*! \fn void guhserver::LogEngine::logEntryAdded(const LogEntry &logEntry);
    This signal is emitted when an \a logEntry was added to the database.

    \sa LogEntry
*/

/*! \fn void guhserver::LogEngine::logDatabaseUpdated();
    This signal is emitted when the log database was updated. The log database
    will be updated when a \l{LogEntry} was added or when a device was removed
    and all corresponding \l{LogEntry}{LogEntries} were removed from the database.
*/

/*!
    \class guhserver::Logging
    \brief  The logging class provides enums and flags for the LogEngine.

    \ingroup logs
    \inmodule core

    \sa LogEngine, LogEntry, LogFilter
*/

/*! \enum guhserver::Logging::LoggingError
    Represents the possible errors from the \l{LogEngine}.

    \value LoggingErrorNoError
        No error happened. Everything is fine.
    \value LoggingErrorLogEntryNotFound
        The requested \l{LogEntry} could not be found.
    \value LoggingErrorInvalidFilterParameter
        The given \l{LogFilter} contains an invalid paramter.
*/

/*! \enum guhserver::Logging::LoggingEventType
    Represents the event type of this \l{LogEntry}.

    \value LoggingEventTypeTrigger
        This event type describes an \l{Event} which has triggered.
    \value LoggingEventTypeActiveChange
        This event type describes an \l{Rule} which has changed its active status.
*/

/*! \enum guhserver::Logging::LoggingLevel
    Indicates if the corresponding \l{LogEntry} is an information or an alert.

    \value LoggingLevelInfo
        This \l{LogEntry} represents an information.
    \value LoggingLevelAlert
        This \l{LogEntry} represents an alert. Something is not ok.
*/

/*! \enum guhserver::Logging::LoggingSource
    Indicates from where the \l{LogEntry} was created. Can be used as flag.

    \value LoggingSourceSystem
        This \l{LogEntry} was created from the guh system (server).
    \value LoggingSourceEvents
        This \l{LogEntry} was created from an \l{Event} which trigged.
    \value LoggingSourceActions
        This \l{LogEntry} was created from an \l{Action} which was executed.
    \value LoggingSourceStates
        This \l{LogEntry} was created from an \l{State} which hase changed.
    \value LoggingSourceRules
        This \l{LogEntry} was created from an \l{Rule} which hase changed the active state or triggered.
*/

#include "guhsettings.h"
#include "logengine.h"
#include "loggingcategories.h"
#include "logging.h"

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QMetaEnum>
#include <QDateTime>

#define DB_SCHEMA_VERSION 2

namespace guhserver {

/*! Constructs the log engine with the given \a parent. */
LogEngine::LogEngine(QObject *parent):
    QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(GuhSettings::logPath());
    m_dbMaxSize = 8000;

    if (QCoreApplication::instance()->organizationName() == "guh-test") {
        m_dbMaxSize = 20;
        qCDebug(dcLogEngine) << "Set logging dab max size to" << m_dbMaxSize << "for testing.";
    }

    qCDebug(dcLogEngine) << "Opening logging database" << m_db.databaseName();

    if (!m_db.isValid()) {
        qCWarning(dcLogEngine) << "Database not valid:" << m_db.lastError().driverText() << m_db.lastError().databaseText();
        return;
    }
    if (!m_db.open()) {
        qCWarning(dcLogEngine) << "Error opening log database:" << m_db.lastError().driverText() << m_db.lastError().databaseText();
        return;
    }

    initDB();
}

LogEngine::~LogEngine()
{
    qCDebug(dcApplication) << "Shutting down \"Log Engine\"";
    m_db.close();
}

/*! Returns the list of \l{LogEntry}{LogEntries} of the database matching the given \a filter.

  \sa LogEntry, LogFilter
*/
QList<LogEntry> LogEngine::logEntries(const LogFilter &filter) const
{
    qCDebug(dcLogEngine) << "Read logging database" << m_db.databaseName();

    QList<LogEntry> results;
    QSqlQuery query;

    QString queryCall = "SELECT * FROM entries ORDER BY timestamp;";
    if (filter.isEmpty()) {
        query.exec(queryCall);
    } else {
        queryCall = QString("SELECT * FROM entries WHERE %1 ORDER BY timestamp;").arg(filter.queryString());
        query.exec(queryCall);
    }

    if (query.lastError().isValid()) {
        qCWarning(dcLogEngine) << "Error fetching log entries. Driver error:" << query.lastError().driverText() << "Database error:" << query.lastError().databaseText();
        return QList<LogEntry>();
    }

    while (query.next()) {
        LogEntry entry(
                    QDateTime::fromTime_t(query.value("timestamp").toLongLong()),
                    (Logging::LoggingLevel)query.value("loggingLevel").toInt(),
                    (Logging::LoggingSource)query.value("sourceType").toInt(),
                    query.value("errorCode").toInt());
        entry.setTypeId(query.value("typeId").toUuid());
        entry.setDeviceId(DeviceId(query.value("deviceId").toString()));
        entry.setValue(query.value("value").toString());
        if ((Logging::LoggingEventType)query.value("loggingEventType").toInt() == Logging::LoggingEventTypeActiveChange) {
            entry.setActive(query.value("active").toBool());
        }
        //qCDebug(dcLogEngine) << entry;
        results.append(entry);
    }
    qCDebug(dcLogEngine) << "Fetched" << results.count() << "entries for db query:" << queryCall;

    return results;
}

/*! Removes all entries from the database. This method will be used for the tests. */
void LogEngine::clearDatabase()
{
    qCWarning(dcLogEngine) << "Clear logging database.";

    QSqlQuery query;
    QString queryDeleteString = QString("DELETE FROM entries;");
    if (!query.exec(queryDeleteString)) {
        qCWarning(dcLogEngine) << "Could not clear logging database. Driver error:" << query.lastError().driverText() << "Database error:" << query.lastError().databaseText();
    }

    emit logDatabaseUpdated();
}

void LogEngine::logSystemEvent(bool active, Logging::LoggingLevel level)
{
    LogEntry entry(level, Logging::LoggingSourceSystem);
    entry.setActive(active);
    appendLogEntry(entry);
}

void LogEngine::logEvent(const Event &event)
{
    QStringList valueList;
    Logging::LoggingSource sourceType;
    if (event.isStateChangeEvent()) {
        sourceType = Logging::LoggingSourceStates;
        valueList << event.param("value").value().toString();
    } else {
        sourceType = Logging::LoggingSourceEvents;
        foreach (const Param &param, event.params()) {
            valueList << param.value().toString();
        }
    }
    LogEntry entry(sourceType);
    entry.setTypeId(event.eventTypeId());
    entry.setDeviceId(event.deviceId());
    entry.setValue(valueList.join(", "));
    appendLogEntry(entry);
}

void LogEngine::logAction(const Action &action, Logging::LoggingLevel level, int errorCode)
{
    QStringList valueList;
    foreach (const Param &param, action.params()) {
        valueList << param.value().toString();
    }
    LogEntry entry(level, Logging::LoggingSourceActions, errorCode);
    entry.setTypeId(action.actionTypeId());
    entry.setDeviceId(action.deviceId());
    entry.setValue(valueList.join(", "));
    appendLogEntry(entry);
}

void LogEngine::logRuleTriggered(const Rule &rule)
{
    LogEntry entry(Logging::LoggingSourceRules);
    entry.setTypeId(rule.id());
    appendLogEntry(entry);
}

void LogEngine::logRuleActiveChanged(const Rule &rule)
{
    LogEntry entry(Logging::LoggingSourceRules);
    entry.setTypeId(rule.id());
    entry.setActive(rule.active());
    appendLogEntry(entry);
}

void LogEngine::removeDeviceLogs(const DeviceId &deviceId)
{
    qCDebug(dcLogEngine) << "Deleting log entries from device" << deviceId.toString();

    QSqlQuery query;
    QString queryDeleteString = QString("DELETE FROM entries WHERE deviceId = '%1';").arg(deviceId.toString());
    if (!query.exec(queryDeleteString)) {
        qCWarning(dcLogEngine) << "Error deleting log entries from device" << deviceId.toString() << ". Driver error:" << query.lastError().driverText() << "Database error:" << query.lastError().databaseText();
    } else {
        emit logDatabaseUpdated();
    }
}

void LogEngine::removeRuleLogs(const RuleId &ruleId)
{
    qCDebug(dcLogEngine) << "Deleting log entries from rule" << ruleId.toString();

    QSqlQuery query;
    QString queryDeleteString = QString("DELETE FROM entries WHERE typeId = '%1';").arg(ruleId.toString());
    if (!query.exec(queryDeleteString)) {
        qCWarning(dcLogEngine) << "Error deleting log entries from rule" << ruleId.toString() << ". Driver error:" << query.lastError().driverText() << "Database error:" << query.lastError().databaseText();
    } else {
        emit logDatabaseUpdated();
    }
}

void LogEngine::appendLogEntry(const LogEntry &entry)
{
    checkDBSize();

    QString queryString = QString("INSERT INTO entries (timestamp, loggingEventType, loggingLevel, sourceType, typeId, deviceId, value, active, errorCode) values ('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
            .arg(entry.timestamp().toTime_t())
            .arg(entry.eventType())
            .arg(entry.level())
            .arg(entry.source())
            .arg(entry.typeId().toString())
            .arg(entry.deviceId().toString())
            .arg(entry.value())
            .arg(entry.active())
            .arg(entry.errorCode());

    QSqlQuery query;
    if (!query.exec(queryString)) {
        qCWarning(dcLogEngine) << "Error writing log entry. Driver error:" << query.lastError().driverText() << "Database error:" << query.lastError().databaseText();
        qCWarning(dcLogEngine) << entry;
        return;
    }

    emit logEntryAdded(entry);
}

void LogEngine::checkDBSize()
{
    QString queryString = "SELECT ROWID FROM entries;";
    QSqlQuery query;
    query.exec(queryString);
    int numRows = 0;
    if (m_db.driver()->hasFeature(QSqlDriver::QuerySize)) {
        numRows = query.size();
    } else {
        // this can be very slow
        query.last();
        numRows = query.at() + 1;
    }

    if (numRows >= m_dbMaxSize) {
        // keep only the latest m_dbMaxSize entries
        qCDebug(dcLogEngine) << "Deleting oldest entries and keep only the latest" << m_dbMaxSize << "entries.";
        QString queryDeleteString = QString("DELETE FROM entries WHERE ROWID IN (SELECT ROWID FROM entries ORDER BY timestamp DESC LIMIT -1 OFFSET %1);").arg(QString::number(m_dbMaxSize));
        if (!query.exec(queryDeleteString)) {
            qCWarning(dcLogEngine) << "Error deleting oldest log entries to keep size. Driver error:" << query.lastError().driverText() << "Database error:" << query.lastError().databaseText();
        } else {
            emit logDatabaseUpdated();
        }
    }
}

void LogEngine::initDB()
{
    m_db.close();
    m_db.open();

    QSqlQuery query;

    if (!m_db.tables().contains("metadata")) {
        query.exec("CREATE TABLE metadata (key varchar(10), data varchar(40));");
        query.exec(QString("INSERT INTO metadata (key, data) VALUES('version', '%1');").arg(DB_SCHEMA_VERSION));
    }

    query.exec("SELECT data FROM metadata WHERE key = 'version';");
    if (query.next()) {
        int version = query.value("data").toInt();
        if (version != DB_SCHEMA_VERSION) {
            qCWarning(dcLogEngine) << "Log schema version not matching! Schema upgrade not implemented yet. Logging might fail.";
        } else {
            qCDebug(dcLogEngine) << QString("Log database schema version \"%1\" matches").arg(DB_SCHEMA_VERSION);
        }
    } else {
        qCWarning(dcLogEngine) << "Broken log database. Version not found in metadata table.";
    }

    if (!m_db.tables().contains("sourceTypes")) {
        query.exec("CREATE TABLE sourceTypes (id int, name varchar(20), PRIMARY KEY(id));");
        //qCDebug(dcLogEngine) << query.lastError().databaseText();
        QMetaEnum logTypes = Logging::staticMetaObject.enumerator(Logging::staticMetaObject.indexOfEnumerator("LoggingSource"));
        Q_ASSERT_X(logTypes.isValid(), "LogEngine", "Logging has no enum LoggingSource");
        for (int i = 0; i < logTypes.keyCount(); i++) {
            query.exec(QString("INSERT INTO sourceTypes (id, name) VALUES(%1, '%2');").arg(i).arg(logTypes.key(i)));
        }
    }

    if (!m_db.tables().contains("loggingEventTypes")) {
        query.exec("CREATE TABLE loggingEventTypes (id int, name varchar(20), PRIMARY KEY(id));");
        //qCDebug(dcLogEngine) << query.lastError().databaseText();
        QMetaEnum logTypes = Logging::staticMetaObject.enumerator(Logging::staticMetaObject.indexOfEnumerator("LoggingEventType"));
        Q_ASSERT_X(logTypes.isValid(), "LogEngine", "Logging has no enum LoggingEventType");
        for (int i = 0; i < logTypes.keyCount(); i++) {
            query.exec(QString("INSERT INTO loggingEventTypes (id, name) VALUES(%1, '%2');").arg(i).arg(logTypes.key(i)));
        }
    }

    if (!m_db.tables().contains("entries")) {
        query.exec("CREATE TABLE entries "
                   "("
                   "timestamp int,"
                   "loggingLevel int,"
                   "sourceType int,"
                   "typeId varchar(38),"
                   "deviceId varchar(38),"
                   "value varchar(100),"
                   "loggingEventType int,"
                   "active bool,"
                   "errorCode int,"
                   "FOREIGN KEY(sourceType) REFERENCES sourceTypes(id),"
                   "FOREIGN KEY(loggingEventType) REFERENCES loggingEventTypes(id)"
                   ");");
        if (query.lastError().isValid()) {
            qCWarning(dcLogEngine) << "Error creating log table in database. Driver error:" << query.lastError().driverText() << "Database error:" << query.lastError().databaseText();
        }
    }
    qCDebug(dcLogEngine) << "Initialized logging DB successfully. (maximum DB size:" << m_dbMaxSize << ")";
}

}
