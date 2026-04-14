#include "viewmodel/MainViewModel.h"

#include "infrastructure/logging/Logging.h"
#include "model/AppModel.h"
#include "model/repository/TaskRepository.h"
#include "model/task/Task.h"
#include "viewmodel/GlobalStateViewModel.h"
#include "viewmodel/TaskActionViewModel.h"
#include "viewmodel/TaskListViewModel.h"

MainViewModel::MainViewModel(AppModel *appModel, TaskRepository *taskRepository, QObject *parent)
    : QObject(parent)
    , m_appModel(appModel)
    , m_taskListViewModel(new TaskListViewModel(taskRepository, this))
    , m_taskActionViewModel(new TaskActionViewModel(taskRepository, this))
    , m_globalStateViewModel(new GlobalStateViewModel(this))
{
    Q_ASSERT(m_appModel != nullptr);
    Q_ASSERT(taskRepository != nullptr);

    connect(m_appModel, &AppModel::counterChanged, this, &MainViewModel::counterChanged);
    connect(m_taskListViewModel,
            &TaskListViewModel::tasksChanged,
            this,
            &MainViewModel::tasksChanged);
    connect(m_globalStateViewModel,
            &GlobalStateViewModel::lastErrorChanged,
            this,
            &MainViewModel::lastDbErrorChanged);
    connect(m_globalStateViewModel,
            &GlobalStateViewModel::errorOccurred,
            this,
            &MainViewModel::errorOccurred);
    connect(m_taskActionViewModel,
            &TaskActionViewModel::operationFailed,
            this,
            &MainViewModel::onChildOperationFailed);
    connect(m_taskListViewModel,
            &TaskListViewModel::operationFailed,
            this,
            &MainViewModel::onChildOperationFailed);

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
    return m_taskListViewModel->tasks();
}

QString MainViewModel::lastDbError() const
{
    return m_globalStateViewModel->lastError();
}

QStringList MainViewModel::taskStatusOptions() const
{
    return taskStatusDisplayOptions();
}

QVariantMap MainViewModel::taskFields() const
{
    return taskFieldMap();
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
    return runTaskMutation([&] {
        return m_taskActionViewModel->create(name, description, dueAtIso, status, autoDelay);
    });
}

bool MainViewModel::loadTasks()
{
    const bool succeeded = m_taskListViewModel->loadTasks();
    if (succeeded) {
        m_globalStateViewModel->clearError();
    }
    return succeeded;
}

QVariantMap MainViewModel::get(qint64 id)
{
    return m_taskListViewModel->get(id);
}

QVariantList MainViewModel::getAll()
{
    return m_taskListViewModel->getAll();
}

bool MainViewModel::updateStatus(qint64 id, int status)
{
    return runTaskMutation([&] { return m_taskActionViewModel->updateStatus(id, status); });
}

bool MainViewModel::remove(qint64 id)
{
    return runTaskMutation([&] { return m_taskActionViewModel->remove(id); });
}

bool MainViewModel::runTaskMutation(const std::function<bool()> &mutation)
{
    if (!mutation()) {
        return false;
    }

    const bool refreshed = m_taskListViewModel->reloadAfterMutation();
    if (!refreshed) {
        app::logging::error("Reload tasks failed after mutation: {}",
                            m_globalStateViewModel->lastError().toStdString());
        return false;
    }

    m_globalStateViewModel->clearError();
    app::logging::info("Task mutation succeeded and routed by MainViewModel");
    return true;
}

void MainViewModel::onChildOperationFailed(const QString &operation,
                                           const QString &detail,
                                           const bool notifyUser)
{
    m_globalStateViewModel->reportOperationError(operation, detail, notifyUser);
    app::logging::error("Child VM operation failed: {}",
                        m_globalStateViewModel->lastError().toStdString());
}
