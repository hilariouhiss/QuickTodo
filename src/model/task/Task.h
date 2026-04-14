#pragma once

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QVariantMap>

#include <optional>

enum class TaskStatus : int { NotStarted = 0, InProgress = 1, Suspended = 2, Completed = 3 };

namespace task::field {
inline constexpr auto Id = "id";
inline constexpr auto Name = "name";
inline constexpr auto Description = "description";
inline constexpr auto DueAt = "dueAt";
inline constexpr auto Status = "status";
inline constexpr auto StatusText = "statusText";
inline constexpr auto CreatedAt = "createdAt";
inline constexpr auto CompletedAt = "completedAt";
inline constexpr auto AutoDelay = "autoDelay";
} // namespace task::field

namespace task::db {
inline constexpr auto Id = "id";
inline constexpr auto Name = "name";
inline constexpr auto Description = "description";
inline constexpr auto DueAt = "due_at";
inline constexpr auto Status = "status";
inline constexpr auto CreatedAt = "created_at";
inline constexpr auto CompletedAt = "completed_at";
inline constexpr auto AutoDelay = "auto_delay";
} // namespace task::db

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
QStringList taskStatusDisplayOptions();
QVariantMap taskFieldMap();
