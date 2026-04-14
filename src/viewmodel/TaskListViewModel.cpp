#include "viewmodel/TaskListViewModel.h"

#include "model/repository/TaskRepository.h"
#include "model/task/Task.h"

TaskListViewModel::TaskListViewModel(TaskRepository *taskRepository, QObject *parent)
    : QObject(parent)
    , m_taskRepository(taskRepository)
{
    Q_ASSERT(m_taskRepository != nullptr);
}

QVariantList TaskListViewModel::tasks() const
{
    return m_tasks;
}

bool TaskListViewModel::loadTasks()
{
    const QList<Task> tasks = m_taskRepository->listTasks();
    QVariantList result;
    result.reserve(tasks.size());
    for (const Task &task : tasks) {
        result.push_back(taskToMap(task));
    }
    setTasks(result);

    if (!m_taskRepository->lastError().isEmpty()) {
        emit operationFailed(QStringLiteral("加载任务失败"), m_taskRepository->lastError(), true);
        return false;
    }

    return true;
}

QVariantMap TaskListViewModel::get(qint64 id)
{
    const auto task = m_taskRepository->getTaskById(id);
    if (!task.has_value()) {
        emit operationFailed(QStringLiteral("查询任务失败"), m_taskRepository->lastError(), false);
        return {};
    }
    return taskToMap(task.value());
}

QVariantList TaskListViewModel::getAll()
{
    if (!loadTasks()) {
        return {};
    }
    return m_tasks;
}

void TaskListViewModel::setTasks(const QVariantList &tasks)
{
    if (m_tasks == tasks) {
        return;
    }
    m_tasks = tasks;
    emit tasksChanged();
}

QVariantMap TaskListViewModel::taskToMap(const Task &task) const
{
    QVariantMap data;
    data.insert(QStringLiteral("id"), task.id);
    data.insert(QStringLiteral("name"), task.name);
    data.insert(QStringLiteral("description"), task.description);
    data.insert(QStringLiteral("dueAt"), task.dueAt.toString(Qt::ISODate));
    data.insert(QStringLiteral("status"), taskStatusToInt(task.status));
    data.insert(QStringLiteral("statusText"), taskStatusToString(task.status));
    data.insert(QStringLiteral("createdAt"), task.createdAt.toString(Qt::ISODate));
    data.insert(QStringLiteral("completedAt"),
                task.completedAt.isValid() ? QVariant(task.completedAt.toString(Qt::ISODate))
                                           : QVariant());
    data.insert(QStringLiteral("autoDelay"), task.autoDelay);
    return data;
}
