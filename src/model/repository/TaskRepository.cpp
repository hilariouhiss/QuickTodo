#include "model/repository/TaskRepository.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <utility>

namespace {
/**
 * @brief Serializes a timestamp to the ISO8601 format stored in SQLite.
 */
QString toIso(const QDateTime &value)
{
    return value.toString(Qt::ISODate);
}

/**
 * @brief Parses a timestamp read from SQLite ISO8601 text columns.
 */
QDateTime fromIso(const QString &value)
{
    return QDateTime::fromString(value, Qt::ISODate);
}
} // namespace

TaskRepository::TaskRepository(QString connectionName)
    : m_connectionName(std::move(connectionName))
{}

/**
 * @brief Inserts a new task row and backfills generated metadata into `task`.
 * @param task Mutable task payload to persist.
 * @return `true` when the row is stored successfully; otherwise `false` and `lastError()`.
 */
bool TaskRepository::insertTask(Task &task)
{
    setLastError(QString());
    if (!ensureDatabaseAvailable()) {
        return false;
    }

    if (!task.dueAt.isValid()) {
        setLastError(QStringLiteral("预计完成时间无效"));
        return false;
    }

    QDateTime createdAt = task.createdAt;
    if (!createdAt.isValid()) {
        createdAt = QDateTime::currentDateTimeUtc();
    }

    /// Persist all timestamps in UTC so QML and storage share one time base.
    QSqlQuery query(database());
    query.prepare(QStringLiteral(
        "INSERT INTO tasks(name, description, due_at, status, created_at, completed_at, "
        "auto_delay) "
        "VALUES(:name, :description, :due_at, :status, :created_at, :completed_at, :auto_delay)"));
    query.bindValue(QStringLiteral(":name"), task.name);
    query.bindValue(QStringLiteral(":description"), task.description);
    query.bindValue(QStringLiteral(":due_at"), toIso(task.dueAt.toUTC()));
    query.bindValue(QStringLiteral(":status"), taskStatusToInt(task.status));
    query.bindValue(QStringLiteral(":created_at"), toIso(createdAt.toUTC()));
    query.bindValue(QStringLiteral(":completed_at"),
                    task.completedAt.isValid() ? QVariant(toIso(task.completedAt.toUTC()))
                                               : QVariant(QVariant::String));
    query.bindValue(QStringLiteral(":auto_delay"), task.autoDelay ? 1 : 0);

    if (!query.exec()) {
        setLastError(query.lastError().text());
        return false;
    }

    task.id = query.lastInsertId().toLongLong();
    task.createdAt = createdAt.toUTC();
    return true;
}

/**
 * @brief Loads a single task by primary key.
 * @param id Task identifier.
 * @return Parsed task on success; otherwise `std::nullopt` and `lastError()`.
 */
std::optional<Task> TaskRepository::getTaskById(qint64 id) const
{
    setLastError(QString());
    if (!ensureDatabaseAvailable()) {
        return std::nullopt;
    }

    QSqlQuery query(database());
    query.prepare(QStringLiteral(
        "SELECT id, name, description, due_at, status, created_at, completed_at, auto_delay "
        "FROM tasks WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);
    if (!query.exec()) {
        setLastError(query.lastError().text());
        return std::nullopt;
    }

    if (!query.next()) {
        setLastError(QStringLiteral("未找到 Task id=%1").arg(id));
        return std::nullopt;
    }
    return parseTaskFromQuery(query);
}

/**
 * @brief Lists all tasks ordered by newest identifier first.
 * @return Parsed task collection. Invalid rows are skipped after recording `lastError()`.
 */
QList<Task> TaskRepository::listTasks() const
{
    setLastError(QString());
    QList<Task> result;
    if (!ensureDatabaseAvailable()) {
        return result;
    }

    QSqlQuery query(database());
    if (!query.exec(QStringLiteral(
            "SELECT id, name, description, due_at, status, created_at, completed_at, auto_delay "
            "FROM tasks ORDER BY id DESC"))) {
        setLastError(query.lastError().text());
        return result;
    }

    while (query.next()) {
        const auto parsed = parseTaskFromQuery(query);
        if (parsed.has_value()) {
            result.append(parsed.value());
        }
    }
    return result;
}

/**
 * @brief Persists the full task snapshot for an existing row.
 * @param task Task payload containing the target `id`.
 * @return `true` when an existing row is updated; otherwise `false`.
 */
bool TaskRepository::updateTask(const Task &task)
{
    setLastError(QString());
    if (!ensureDatabaseAvailable()) {
        return false;
    }

    if (task.id <= 0) {
        setLastError(QStringLiteral("Task id 非法"));
        return false;
    }

    QSqlQuery query(database());
    query.prepare(QStringLiteral("UPDATE tasks SET name = :name, description = :description, "
                                 "due_at = :due_at, status = :status, "
                                 "created_at = :created_at, completed_at = :completed_at, "
                                 "auto_delay = :auto_delay WHERE id = :id"));
    query.bindValue(QStringLiteral(":name"), task.name);
    query.bindValue(QStringLiteral(":description"), task.description);
    query.bindValue(QStringLiteral(":due_at"), toIso(task.dueAt.toUTC()));
    query.bindValue(QStringLiteral(":status"), taskStatusToInt(task.status));
    query.bindValue(QStringLiteral(":created_at"), toIso(task.createdAt.toUTC()));
    query.bindValue(QStringLiteral(":completed_at"),
                    task.completedAt.isValid() ? QVariant(toIso(task.completedAt.toUTC()))
                                               : QVariant(QVariant::String));
    query.bindValue(QStringLiteral(":auto_delay"), task.autoDelay ? 1 : 0);
    query.bindValue(QStringLiteral(":id"), task.id);

    if (!query.exec()) {
        setLastError(query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setLastError(QStringLiteral("更新失败，Task 不存在 id=%1").arg(task.id));
        return false;
    }

    return true;
}

/**
 * @brief Removes a task row by identifier.
 * @param id Task identifier.
 * @return `true` when a row is deleted; otherwise `false`.
 */
bool TaskRepository::deleteTask(qint64 id)
{
    setLastError(QString());
    if (!ensureDatabaseAvailable()) {
        return false;
    }

    QSqlQuery query(database());
    query.prepare(QStringLiteral("DELETE FROM tasks WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        setLastError(query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setLastError(QStringLiteral("删除失败，Task 不存在 id=%1").arg(id));
        return false;
    }

    return true;
}

void TaskRepository::setConnectionName(const QString &connectionName)
{
    m_connectionName = connectionName;
}

QString TaskRepository::lastError() const
{
    return m_lastError;
}

/**
 * @brief Verifies that the configured Qt SQL connection exists and is open.
 * @return `true` when repository queries can be executed safely.
 */
bool TaskRepository::ensureDatabaseAvailable() const
{
    if (!QSqlDatabase::contains(m_connectionName)) {
        setLastError(QStringLiteral("数据库连接不存在: %1").arg(m_connectionName));
        return false;
    }

    QSqlDatabase db = database();
    if (!db.isValid()) {
        setLastError(QStringLiteral("数据库连接无效: %1").arg(m_connectionName));
        return false;
    }

    if (!db.isOpen() && !db.open()) {
        setLastError(db.lastError().text());
        return false;
    }

    return true;
}

QSqlDatabase TaskRepository::database() const
{
    return QSqlDatabase::database(m_connectionName);
}

void TaskRepository::setLastError(const QString &errorText) const
{
    m_lastError = errorText;
}

/**
 * @brief Maps the current SQL result row into the `Task` domain model.
 * @param query Query positioned on a valid row.
 * @return Parsed task, or `std::nullopt` when persisted enum values are invalid.
 */
std::optional<Task> TaskRepository::parseTaskFromQuery(const QSqlQuery &query) const
{
    /// Convert one result row into the domain model and reject unknown status values.
    Task task;
    task.id = query.value(QString::fromLatin1(task::db::Id)).toLongLong();
    task.name = query.value(QString::fromLatin1(task::db::Name)).toString();
    task.description = query.value(QString::fromLatin1(task::db::Description)).toString();
    task.dueAt = fromIso(query.value(QString::fromLatin1(task::db::DueAt)).toString());
    task.createdAt = fromIso(query.value(QString::fromLatin1(task::db::CreatedAt)).toString());
    task.completedAt = fromIso(query.value(QString::fromLatin1(task::db::CompletedAt)).toString());
    task.autoDelay = query.value(QString::fromLatin1(task::db::AutoDelay)).toInt() != 0;

    const int statusValue = query.value(QString::fromLatin1(task::db::Status)).toInt();
    const auto status = taskStatusFromInt(statusValue);
    if (!status.has_value()) {
        setLastError(QStringLiteral("读取到非法状态值: %1").arg(statusValue));
        return std::nullopt;
    }
    task.status = status.value();

    return task;
}
