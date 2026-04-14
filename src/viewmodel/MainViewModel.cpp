#include "viewmodel/MainViewModel.h"

#include "infrastructure/logging/Logging.h"
#include "model/AppModel.h"
#include "model/repository/TaskRepository.h"
#include "model/task/Task.h"

MainViewModel::MainViewModel(AppModel *appModel, TaskRepository *taskRepository, QObject *parent)
    : QObject(parent)
    , m_appModel(appModel)
    , m_taskRepository(taskRepository)
{
    Q_ASSERT(m_appModel != nullptr);
    Q_ASSERT(m_taskRepository != nullptr);
    connect(m_appModel, &AppModel::counterChanged, this, &MainViewModel::counterChanged);
    app::logging::info("MainViewModel initialized");
}

int MainViewModel::counter() const
{
    return m_appModel->counter();
}

QString MainViewModel::pageTitle() const
{
    return QStringLiteral("Quick Todo");
}

QVariantList MainViewModel::tasks() const
{
    return m_tasks;
}

QString MainViewModel::lastDbError() const
{
    return m_lastDbError;
}

void MainViewModel::incrementCounter()
{
    m_appModel->setCounter(m_appModel->counter() + 1);
    app::logging::info("Counter incremented to {}", m_appModel->counter());
}

void MainViewModel::resetCounter()
{
    m_appModel->setCounter(0);
    app::logging::info("Counter reset");
}

bool MainViewModel::create(const QString &name,
                           const QString &description,
                           const QString &dueAtIso,
                           const int status,
                           const bool autoDelay)
{
    if (name.trimmed().isEmpty()) {
        return reportOperationError(QStringLiteral("创建任务失败"), QStringLiteral("名称不能为空"));
    }

    const QDateTime dueAt = QDateTime::fromString(dueAtIso, Qt::ISODate);
    if (!dueAt.isValid()) {
        return reportOperationError(QStringLiteral("创建任务失败"),
                                    QStringLiteral("预计完成时间格式非法，需为 ISO8601"));
    }

    const auto statusEnum = taskStatusFromInt(status);
    if (!statusEnum.has_value()) {
        return reportOperationError(QStringLiteral("创建任务失败"),
                                    QStringLiteral("Task 状态非法: %1").arg(status));
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
        const bool result = reportOperationError(QStringLiteral("创建任务失败"),
                                                 m_taskRepository->lastError());
        app::logging::error("Create task failed: {}", m_lastDbError.toStdString());
        return result;
    }

    setLastDbError(QString());
    app::logging::info("Task created: id={}, name={}", task.id, task.name.toStdString());
    return loadTasks();
}

bool MainViewModel::loadTasks()
{
    const QList<Task> tasks = m_taskRepository->listTasks();
    QVariantList result;
    result.reserve(tasks.size());
    for (const Task &task : tasks) {
        result.push_back(taskToMap(task));
    }
    setTasks(result);

    if (!m_taskRepository->lastError().isEmpty()) {
        return reportOperationError(QStringLiteral("加载任务失败"), m_taskRepository->lastError());
    }

    setLastDbError(QString());
    return true;
}

QVariantMap MainViewModel::get(qint64 id)
{
    const auto task = m_taskRepository->getTaskById(id);
    if (!task.has_value()) {
        setLastDbError(m_taskRepository->lastError());
        return {};
    }

    setLastDbError(QString());
    return taskToMap(task.value());
}

QVariantList MainViewModel::getAll()
{
    if (!loadTasks()) {
        return {};
    }
    return m_tasks;
}

bool MainViewModel::updateStatus(qint64 id, int status)
{
    const auto statusEnum = taskStatusFromInt(status);
    if (!statusEnum.has_value()) {
        return reportOperationError(QStringLiteral("更新任务状态失败"),
                                    QStringLiteral("Task 状态非法: %1").arg(status));
    }

    auto taskOpt = m_taskRepository->getTaskById(id);
    if (!taskOpt.has_value()) {
        return reportOperationError(QStringLiteral("更新任务状态失败"),
                                    m_taskRepository->lastError());
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
        return reportOperationError(QStringLiteral("更新任务状态失败"),
                                    m_taskRepository->lastError());
    }

    setLastDbError(QString());
    app::logging::info("Task status updated: id={}, status={}", id, status);
    return loadTasks();
}

bool MainViewModel::remove(qint64 id)
{
    if (!m_taskRepository->deleteTask(id)) {
        return reportOperationError(QStringLiteral("删除任务失败"), m_taskRepository->lastError());
    }

    setLastDbError(QString());
    app::logging::info("Task removed: id={}", id);
    return loadTasks();
}

void MainViewModel::setLastDbError(const QString &value)
{
    if (m_lastDbError == value) {
        return;
    }
    m_lastDbError = value;
    emit lastDbErrorChanged();
}

bool MainViewModel::reportOperationError(const QString &operation, const QString &detail)
{
    QString message = operation;
    if (!detail.trimmed().isEmpty()) {
        message = QStringLiteral("%1：%2").arg(operation, detail);
    }
    setLastDbError(message);
    emit errorOccurred(message);
    return false;
}

void MainViewModel::setTasks(const QVariantList &tasks)
{
    if (m_tasks == tasks) {
        return;
    }
    m_tasks = tasks;
    emit tasksChanged();
}

QVariantMap MainViewModel::taskToMap(const Task &task) const
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
