#pragma once

#include "model/task/Task.h"

#include <QList>
#include <QSqlDatabase>
#include <QString>

#include <optional>

class QSqlQuery;

class TaskRepository final
{
public:
    explicit TaskRepository(QString connectionName = QStringLiteral("main"));

    bool insertTask(Task &task);
    std::optional<Task> getTaskById(qint64 id) const;
    QList<Task> listTasks() const;
    bool updateTask(const Task &task);
    bool deleteTask(qint64 id);

    void setConnectionName(const QString &connectionName);
    QString lastError() const;

private:
    bool ensureDatabaseAvailable() const;
    QSqlDatabase database() const;
    void setLastError(const QString &errorText) const;
    std::optional<Task> parseTaskFromQuery(const QSqlQuery &query) const;

private:
    QString m_connectionName;
    mutable QString m_lastError;
};
