#include "model/task/Task.h"

#include <array>

namespace {
constexpr std::array<TaskStatus, 4> kStatusOrder = {TaskStatus::NotStarted,
                                                    TaskStatus::InProgress,
                                                    TaskStatus::Suspended,
                                                    TaskStatus::Completed};
}

int taskStatusToInt(TaskStatus status)
{
    return static_cast<int>(status);
}

std::optional<TaskStatus> taskStatusFromInt(int value)
{
    switch (value) {
    case 0:
        return TaskStatus::NotStarted;
    case 1:
        return TaskStatus::InProgress;
    case 2:
        return TaskStatus::Suspended;
    case 3:
        return TaskStatus::Completed;
    default:
        return std::nullopt;
    }
}

QString taskStatusToString(TaskStatus status)
{
    switch (status) {
    case TaskStatus::NotStarted:
        return QStringLiteral("未开始");
    case TaskStatus::InProgress:
        return QStringLiteral("进行中");
    case TaskStatus::Suspended:
        return QStringLiteral("挂起");
    case TaskStatus::Completed:
        return QStringLiteral("已完成");
    }
    return QStringLiteral("未知");
}

QStringList taskStatusDisplayOptions()
{
    QStringList options;
    options.reserve(static_cast<qsizetype>(kStatusOrder.size()));
    for (const TaskStatus status : kStatusOrder) {
        options.append(taskStatusToString(status));
    }
    return options;
}

QVariantMap taskFieldMap()
{
    QVariantMap fields;
    fields.insert(QStringLiteral("id"), QString::fromLatin1(task::field::Id));
    fields.insert(QStringLiteral("name"), QString::fromLatin1(task::field::Name));
    fields.insert(QStringLiteral("description"), QString::fromLatin1(task::field::Description));
    fields.insert(QStringLiteral("dueAt"), QString::fromLatin1(task::field::DueAt));
    fields.insert(QStringLiteral("status"), QString::fromLatin1(task::field::Status));
    fields.insert(QStringLiteral("statusText"), QString::fromLatin1(task::field::StatusText));
    fields.insert(QStringLiteral("createdAt"), QString::fromLatin1(task::field::CreatedAt));
    fields.insert(QStringLiteral("completedAt"), QString::fromLatin1(task::field::CompletedAt));
    fields.insert(QStringLiteral("autoDelay"), QString::fromLatin1(task::field::AutoDelay));
    return fields;
}
