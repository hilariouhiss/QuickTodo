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
        setLastDbError(QStringLiteral("名称不能为空"));
        return false;
    }

    const QDateTime dueAt = QDateTime::fromString(dueAtIso, Qt::ISODate);
    if (!dueAt.isValid()) {
        setLastDbError(QStringLiteral("预计完成时间格式非法，需为 ISO8601"));
        return false;
    }

    const auto statusEnum = taskStatusFromInt(status);
    if (!statusEnum.has_value()) {
        setLastDbError(QStringLiteral("Task 状态非法: %1").arg(status));
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
        setLastDbError(m_taskRepository->lastError());
        app::logging::error("Create task failed: {}", m_lastDbError.toStdString());
        return false;
    }

    setLastDbError(QString());
    app::logging::info("Task created: id={}, name={}", task.id, task.name.toStdString());
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
    const QList<Task> tasks = m_taskRepository->listTasks();
    if (!m_taskRepository->lastError().isEmpty()) {
        setLastDbError(m_taskRepository->lastError());
    } else {
        setLastDbError(QString());
    }

    QVariantList result;
    result.reserve(tasks.size());
    for (const Task &task : tasks) {
        result.push_back(taskToMap(task));
    }
    return result;
}

bool MainViewModel::updateStatus(qint64 id, int status)
{
    const auto statusEnum = taskStatusFromInt(status);
    if (!statusEnum.has_value()) {
        setLastDbError(QStringLiteral("Task 状态非法: %1").arg(status));
        return false;
    }

    auto taskOpt = m_taskRepository->getTaskById(id);
    if (!taskOpt.has_value()) {
        setLastDbError(m_taskRepository->lastError());
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
        setLastDbError(m_taskRepository->lastError());
        return false;
    }

    setLastDbError(QString());
    app::logging::info("Task status updated: id={}, status={}", id, status);
    return true;
}

bool MainViewModel::remove(qint64 id)
{
    if (!m_taskRepository->deleteTask(id)) {
        setLastDbError(m_taskRepository->lastError());
        return false;
    }

    setLastDbError(QString());
    app::logging::info("Task removed: id={}", id);
    return true;
}

void MainViewModel::setLastDbError(const QString &value)
{
    if (m_lastDbError == value) {
        return;
    }
    m_lastDbError = value;
    emit lastDbErrorChanged();
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
