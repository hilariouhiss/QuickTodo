#include "infrastructure/database/DatabaseManager.h"

#include <QDir>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QStringList>
#include <QVariant>

#include "infrastructure/logging/Logging.h"
#include "model/task/Task.h"

#include <array>

namespace {
constexpr auto kCreateTasksTableSql = R"(
CREATE TABLE IF NOT EXISTS tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT NOT NULL DEFAULT '',
    due_at TEXT NOT NULL,
    status INTEGER NOT NULL DEFAULT 0,
    created_at TEXT NOT NULL,
    completed_at TEXT,
    auto_delay INTEGER NOT NULL DEFAULT 0
)
)";

constexpr std::array<const char *, 8> kRequiredColumns
    = {task::db::Id,
       task::db::Name,
       task::db::Description,
       task::db::DueAt,
       task::db::Status,
       task::db::CreatedAt,
       task::db::CompletedAt,
       task::db::AutoDelay};
} // namespace

DatabaseManager::DatabaseManager()
    : m_defaultConnectionName(QStringLiteral("main"))
{}

bool DatabaseManager::initializeDefaultDatabase()
{
    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE"))) {
        setLastError(QStringLiteral("QSQLITE 驱动不可用"));
        app::logging::error("QSQLITE driver is unavailable");
        return false;
    }

    QSqlDatabase db = createConnection(m_defaultConnectionName);
    if (!db.isValid()) {
        return false;
    }

    if (!db.isOpen() && !db.open()) {
        setLastError(db.lastError().text());
        app::logging::error("Open default database failed: {}", m_lastError.toStdString());
        return false;
    }

    if (!ensureTasksTable(db)) {
        return false;
    }

    if (!validateSchema()) {
        return false;
    }

    app::logging::info("Database initialized: {}", db.databaseName().toStdString());
    return true;
}

QSqlDatabase DatabaseManager::createConnection(const QString &connectionName,
                                               const QString &dbFilePath)
{
    if (QSqlDatabase::contains(connectionName)) {
        return QSqlDatabase::database(connectionName);
    }

    const QString filePath = resolveDatabasePath(dbFilePath);
    const QFileInfo fileInfo(filePath);
    QDir parentDir(fileInfo.absolutePath());
    if (!parentDir.exists() && !parentDir.mkpath(QStringLiteral("."))) {
        setLastError(QStringLiteral("数据库目录创建失败: %1").arg(parentDir.absolutePath()));
        app::logging::error("Create database directory failed: {}",
                            parentDir.absolutePath().toStdString());
        return {};
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName);
    db.setDatabaseName(filePath);
    app::logging::info("Database connection prepared: name={}, path={}",
                       connectionName.toStdString(),
                       filePath.toStdString());
    return db;
}

QSqlDatabase DatabaseManager::defaultConnection() const
{
    if (!QSqlDatabase::contains(m_defaultConnectionName)) {
        return {};
    }
    return QSqlDatabase::database(m_defaultConnectionName);
}

bool DatabaseManager::validateSchema()
{
    QSqlDatabase db = defaultConnection();
    if (!db.isValid()) {
        setLastError(QStringLiteral("默认数据库连接无效"));
        return false;
    }

    if (!db.isOpen() && !db.open()) {
        setLastError(db.lastError().text());
        return false;
    }

    if (!ensureRequiredColumns(db)) {
        return false;
    }

    if (!ensureIndexes(db)) {
        return false;
    }

    app::logging::info("Database schema validation passed");
    return true;
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::ensureTasksTable(const QSqlDatabase &db)
{
    QSqlQuery query(db);
    if (!query.exec(QString::fromUtf8(kCreateTasksTableSql))) {
        setLastError(query.lastError().text());
        app::logging::error("Create tasks table failed: {}", m_lastError.toStdString());
        return false;
    }
    return true;
}

bool DatabaseManager::ensureRequiredColumns(const QSqlDatabase &db)
{
    QSqlQuery query(db);
    if (!query.exec(QStringLiteral("PRAGMA table_info(tasks)"))) {
        setLastError(query.lastError().text());
        app::logging::error("Read tasks schema failed: {}", m_lastError.toStdString());
        return false;
    }

    QStringList columns;
    while (query.next()) {
        columns.append(query.value(QStringLiteral("name")).toString());
    }

    for (const auto *column : kRequiredColumns) {
        if (!columns.contains(QLatin1String(column))) {
            setLastError(QStringLiteral("任务表缺失字段: %1").arg(QLatin1String(column)));
            app::logging::error("Database schema missing column: {}", column);
            return false;
        }
    }

    return true;
}

bool DatabaseManager::ensureIndexes(const QSqlDatabase &db)
{
    QSqlQuery query(db);
    if (!query.exec(
            QStringLiteral("CREATE INDEX IF NOT EXISTS idx_tasks_status ON tasks(status)"))) {
        setLastError(query.lastError().text());
        app::logging::error("Create status index failed: {}", m_lastError.toStdString());
        return false;
    }

    if (!query.exec(
            QStringLiteral("CREATE INDEX IF NOT EXISTS idx_tasks_due_at ON tasks(due_at)"))) {
        setLastError(query.lastError().text());
        app::logging::error("Create due_at index failed: {}", m_lastError.toStdString());
        return false;
    }

    return true;
}

void DatabaseManager::setLastError(const QString &errorText)
{
    m_lastError = errorText;
}

QString DatabaseManager::resolveDatabasePath(const QString &dbFilePath) const
{
    if (!dbFilePath.isEmpty()) {
        return dbFilePath;
    }

    const QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return appDataDir + QStringLiteral("/tasks.db");
}
