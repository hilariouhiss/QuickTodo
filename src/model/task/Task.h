#pragma once

#include <QDateTime>
#include <QString>

#include <optional>

enum class TaskStatus : int { NotStarted = 0, InProgress = 1, Suspended = 2, Completed = 3 };

struct Task
{
    qint64 id = 0;
    QString name;
    QString description;
    QDateTime dueAt;
    TaskStatus status = TaskStatus::NotStarted;
    QDateTime createdAt;
    QDateTime completedAt;
    bool autoDelay = false;
};

int taskStatusToInt(TaskStatus status);
std::optional<TaskStatus> taskStatusFromInt(int value);
QString taskStatusToString(TaskStatus status);
