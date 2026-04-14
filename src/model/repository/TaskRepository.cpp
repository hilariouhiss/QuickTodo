#include "model/repository/TaskRepository.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <utility>

namespace {
QString toIso(const QDateTime &value)
{
    return value.toString(Qt::ISODate);
}

QDateTime fromIso(const QString &value)
{
    return QDateTime::fromString(value, Qt::ISODate);
}
} // namespace

TaskRepository::TaskRepository(QString connectionName)
    : m_connectionName(std::move(connectionName))
{}

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

std::optional<Task> TaskRepository::parseTaskFromQuery(const QSqlQuery &query) const
{
    Task task;
    task.id = query.value(QStringLiteral("id")).toLongLong();
    task.name = query.value(QStringLiteral("name")).toString();
    task.description = query.value(QStringLiteral("description")).toString();
    task.dueAt = fromIso(query.value(QStringLiteral("due_at")).toString());
    task.createdAt = fromIso(query.value(QStringLiteral("created_at")).toString());
    task.completedAt = fromIso(query.value(QStringLiteral("completed_at")).toString());
    task.autoDelay = query.value(QStringLiteral("auto_delay")).toInt() != 0;

    const int statusValue = query.value(QStringLiteral("status")).toInt();
    const auto status = taskStatusFromInt(statusValue);
    if (!status.has_value()) {
        setLastError(QStringLiteral("读取到非法状态值: %1").arg(statusValue));
        return std::nullopt;
    }
    task.status = status.value();

    return task;
}
