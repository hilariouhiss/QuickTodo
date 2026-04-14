#include "viewmodel/TaskActionViewModel.h"

#include "model/repository/TaskRepository.h"
#include "model/task/Task.h"

#include <QDateTime>

TaskActionViewModel::TaskActionViewModel(TaskRepository *taskRepository, QObject *parent)
    : QObject(parent)
    , m_taskRepository(taskRepository)
{
    Q_ASSERT(m_taskRepository != nullptr);
}

bool TaskActionViewModel::create(const QString &name,
                                 const QString &description,
                                 const QString &dueAtIso,
                                 const int status,
                                 const bool autoDelay)
{
    if (name.trimmed().isEmpty()) {
        emit operationFailed(QStringLiteral("创建任务失败"), QStringLiteral("名称不能为空"), true);
        return false;
    }

    const QDateTime dueAt = QDateTime::fromString(dueAtIso, Qt::ISODate);
    if (!dueAt.isValid()) {
        emit operationFailed(QStringLiteral("创建任务失败"),
                             QStringLiteral("预计完成时间格式非法，需为 ISO8601"),
                             true);
        return false;
    }

    const auto statusEnum = taskStatusFromInt(status);
    if (!statusEnum.has_value()) {
        emit operationFailed(QStringLiteral("创建任务失败"),
                             QStringLiteral("Task 状态非法: %1").arg(status),
                             true);
        return false;
    }

    Task task;
    task.name = name;
    task.description = description;
    task.dueAt = dueAt.toUTC();
    task.status = statusEnum.value();
    task.createdAt = QDateTime::currentDateTimeUtc();
    task.completedAt = (task.status == TaskStatus::Completed) ? task.createdAt : QDateTime();
    task.autoDelay = autoDelay;

    if (!m_taskRepository->insertTask(task)) {
        emit operationFailed(QStringLiteral("创建任务失败"), m_taskRepository->lastError(), true);
        return false;
    }

    return true;
}

bool TaskActionViewModel::updateStatus(qint64 id, int status)
{
    const auto statusEnum = taskStatusFromInt(status);
    if (!statusEnum.has_value()) {
        emit operationFailed(QStringLiteral("更新任务状态失败"),
                             QStringLiteral("Task 状态非法: %1").arg(status),
                             true);
        return false;
    }

    auto taskOpt = m_taskRepository->getTaskById(id);
    if (!taskOpt.has_value()) {
        emit operationFailed(QStringLiteral("更新任务状态失败"),
                             m_taskRepository->lastError(),
                             true);
        return false;
    }

    Task task = taskOpt.value();
    task.status = statusEnum.value();
    if (task.status == TaskStatus::Completed && !task.completedAt.isValid()) {
        task.completedAt = QDateTime::currentDateTimeUtc();
    }
    if (task.status != TaskStatus::Completed) {
        task.completedAt = QDateTime();
    }

    if (!m_taskRepository->updateTask(task)) {
        emit operationFailed(QStringLiteral("更新任务状态失败"),
                             m_taskRepository->lastError(),
                             true);
        return false;
    }

    return true;
}

bool TaskActionViewModel::remove(qint64 id)
{
    if (!m_taskRepository->deleteTask(id)) {
        emit operationFailed(QStringLiteral("删除任务失败"), m_taskRepository->lastError(), true);
        return false;
    }

    return true;
}
