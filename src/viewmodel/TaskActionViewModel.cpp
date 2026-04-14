#include "viewmodel/TaskActionViewModel.h"

#include "model/repository/TaskRepository.h"
#include "model/task/Task.h"

#include <QDateTime>

/**
 * @brief Wraps repository writes with UI-facing validation and error signaling.
 * @param taskRepository Repository used for task mutations.
 * @param parent Owning QObject parent.
 */
TaskActionViewModel::TaskActionViewModel(TaskRepository *taskRepository, QObject *parent)
    : QObject(parent)
    , m_taskRepository(taskRepository)
{
    Q_ASSERT(m_taskRepository != nullptr);
}

/**
 * @brief Validates form input and creates a new task in the repository.
 * @param name Required task name from QML.
 * @param description Optional task description from QML.
 * @param dueAtIso Due date in ISO8601 format.
 * @param status Numeric status value expected by `TaskStatus`.
 * @param autoDelay Whether automatic delay handling is enabled for the task.
 * @return `true` when the task is created successfully.
 */
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
    /// Keep completion metadata consistent with the initial status selected in QML.
    task.completedAt = (task.status == TaskStatus::Completed) ? task.createdAt : QDateTime();
    task.autoDelay = autoDelay;

    if (!m_taskRepository->insertTask(task)) {
        emit operationFailed(QStringLiteral("创建任务失败"), m_taskRepository->lastError(), true);
        return false;
    }

    return true;
}

/**
 * @brief Updates only the status-derived fields for an existing task.
 * @param id Task identifier.
 * @param status Numeric status value expected by `TaskStatus`.
 * @return `true` when the task exists and the status transition is persisted.
 */
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

/**
 * @brief Deletes a task by identifier.
 * @param id Task identifier.
 * @return `true` when the repository deletion succeeds.
 */
bool TaskActionViewModel::remove(qint64 id)
{
    if (!m_taskRepository->deleteTask(id)) {
        emit operationFailed(QStringLiteral("删除任务失败"), m_taskRepository->lastError(), true);
        return false;
    }

    return true;
}
