#include "model/task/Task.h"

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
