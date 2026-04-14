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

bool TaskListViewModel::reloadAfterMutation()
{
    return loadTasks();
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
    data.insert(QString::fromLatin1(task::field::Id), task.id);
    data.insert(QString::fromLatin1(task::field::Name), task.name);
    data.insert(QString::fromLatin1(task::field::Description), task.description);
    data.insert(QString::fromLatin1(task::field::DueAt), task.dueAt.toString(Qt::ISODate));
    data.insert(QString::fromLatin1(task::field::Status), taskStatusToInt(task.status));
    data.insert(QString::fromLatin1(task::field::StatusText), taskStatusToString(task.status));
    data.insert(QString::fromLatin1(task::field::CreatedAt), task.createdAt.toString(Qt::ISODate));
    data.insert(QString::fromLatin1(task::field::CompletedAt),
                task.completedAt.isValid() ? QVariant(task.completedAt.toString(Qt::ISODate))
                                           : QVariant());
    data.insert(QString::fromLatin1(task::field::AutoDelay), task.autoDelay);
    return data;
}
