#pragma once

#include <QSqlDatabase>
#include <QString>

/**
 * @brief Owns SQLite connection setup and schema validation for the application.
 */
class DatabaseManager final
{
public:
    /**
     * @brief Creates a manager for the default application database connection.
     */
    DatabaseManager();

    /**
     * @brief Initializes the default SQLite database and validates its schema.
     * @return `true` when the default connection is ready for repository use.
     */
    bool initializeDefaultDatabase();

    /**
     * @brief Creates or reuses a named SQLite connection.
     * @param connectionName Qt SQL connection name.
     * @param dbFilePath Optional explicit database path.
     * @return Prepared connection, or an invalid one when setup fails.
     */
    QSqlDatabase createConnection(const QString &connectionName,
                                  const QString &dbFilePath = QString());

    /**
     * @brief Returns the configured default connection when it exists.
     * @return Default connection object, or an invalid one when unavailable.
     */
    QSqlDatabase defaultConnection() const;

    /**
     * @brief Verifies that the default schema matches repository expectations.
     * @return `true` when required tables, columns, and indexes are available.
     */
    bool validateSchema();

    /**
     * @brief Returns the last database setup or validation error.
     * @return Last error text, or an empty string when the previous call succeeded.
     */
    QString lastError() const;

private:
    bool ensureTasksTable(const QSqlDatabase &db);
    bool ensureRequiredColumns(const QSqlDatabase &db);
    bool ensureIndexes(const QSqlDatabase &db);
    void setLastError(const QString &errorText);
    static QString resolveDatabasePath(const QString &dbFilePath);
    QString m_defaultConnectionName;
    QString m_lastError;
};
