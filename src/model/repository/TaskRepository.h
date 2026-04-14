#pragma once

#include "model/task/Task.h"

#include <QList>
#include <QSqlDatabase>
#include <QString>

#include <optional>

class QSqlQuery;

/**
 * @brief Persists and retrieves `Task` domain objects through a named Qt SQL connection.
 */
class TaskRepository final
{
public:
    /**
     * @brief Creates a repository bound to the specified Qt SQL connection.
     * @param connectionName Connection name used by `QSqlDatabase`.
     */
    explicit TaskRepository(QString connectionName = QStringLiteral("main"));

    /**
     * @brief Inserts a task and updates generated fields such as `id`.
     * @param task Mutable task payload to persist.
     * @return `true` on success; otherwise `false` and `lastError()`.
     */
    bool insertTask(Task &task);

    /**
     * @brief Finds a task by primary key.
     * @param id Task identifier.
     * @return Parsed task, or `std::nullopt` when not found or on error.
     */
    std::optional<Task> getTaskById(qint64 id) const;

    /**
     * @brief Lists all persisted tasks.
     * @return Task collection ordered by newest identifier first.
     */
    QList<Task> listTasks() const;

    /**
     * @brief Replaces the persisted snapshot of an existing task.
     * @param task Task payload containing the target `id`.
     * @return `true` when the row exists and is updated successfully.
     */
    bool updateTask(const Task &task);

    /**
     * @brief Deletes a task by identifier.
     * @param id Task identifier.
     * @return `true` when a row is removed successfully.
     */
    bool deleteTask(qint64 id);

    /**
     * @brief Switches the repository to a different Qt SQL connection.
     * @param connectionName Connection name registered in `QSqlDatabase`.
     */
    void setConnectionName(const QString &connectionName);

    /**
     * @brief Returns the last repository error message.
     * @return Last error text, or an empty string when the previous call succeeded.
     */
    QString lastError() const;

private:
    bool ensureDatabaseAvailable() const;
    QSqlDatabase database() const;
    void setLastError(const QString &errorText) const;
    std::optional<Task> parseTaskFromQuery(const QSqlQuery &query) const;
    QString m_connectionName;
    mutable QString m_lastError;
};
