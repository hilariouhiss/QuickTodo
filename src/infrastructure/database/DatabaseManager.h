#pragma once

#include <QSqlDatabase>
#include <QString>

class DatabaseManager final
{
public:
    DatabaseManager();

    bool initializeDefaultDatabase();
    QSqlDatabase createConnection(const QString &connectionName,
                                  const QString &dbFilePath = QString());
    QSqlDatabase defaultConnection() const;
    bool validateSchema();
    QString lastError() const;

private:
    bool ensureTasksTable(const QSqlDatabase &db);
    bool ensureRequiredColumns(const QSqlDatabase &db);
    bool ensureIndexes(const QSqlDatabase &db);
    void setLastError(const QString &errorText);
    QString resolveDatabasePath(const QString &dbFilePath) const;

private:
    QString m_defaultConnectionName;
    QString m_lastError;
};
