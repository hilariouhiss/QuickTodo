#pragma once

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QVariantMap>

#include <optional>

/**
 * @brief Lifecycle states persisted for a task record.
 */
enum class TaskStatus : int { NotStarted = 0, InProgress = 1, Suspended = 2, Completed = 3 };

namespace task::field {
/// @brief Canonical keys exposed to QVariant/QML consumers.
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
/// @brief Canonical column names used by the SQLite schema.
inline constexpr auto Id = "id";
inline constexpr auto Name = "name";
inline constexpr auto Description = "description";
inline constexpr auto DueAt = "due_at";
inline constexpr auto Status = "status";
inline constexpr auto CreatedAt = "created_at";
inline constexpr auto CompletedAt = "completed_at";
inline constexpr auto AutoDelay = "auto_delay";
} // namespace task::db

/**
 * @brief Domain model shared across repository and view-model layers.
 */
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

/**
 * @brief Converts a task status enum to its persisted integer value.
 * @param status Domain status to serialize.
 * @return Integer value stored in the database and view-model payloads.
 */
int taskStatusToInt(TaskStatus status);

/**
 * @brief Converts a persisted integer value back to a task status.
 * @param value Integer value read from storage or UI payloads.
 * @return Parsed status, or `std::nullopt` when the value is not recognized.
 */
std::optional<TaskStatus> taskStatusFromInt(int value);

/**
 * @brief Returns the stable internal name for a task status.
 * @param status Domain status to describe.
 * @return Human-readable status token used in the UI payload.
 */
QString taskStatusToString(TaskStatus status);

/**
 * @brief Lists status labels in the same order as the numeric enum values.
 * @return UI-facing status display options.
 */
QStringList taskStatusDisplayOptions();

/**
 * @brief Builds the field-name map consumed by QML forms and bindings.
 * @return Map of logical task fields keyed by their exported names.
 */
QVariantMap taskFieldMap();
