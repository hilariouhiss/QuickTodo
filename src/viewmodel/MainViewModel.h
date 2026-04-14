#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

#include <functional>

class AppModel;
class TaskRepository;
class TaskListViewModel;
class TaskActionViewModel;
class GlobalStateViewModel;

/**
 * @brief Root QML-facing view-model that coordinates task state, actions, and global errors.
 */
class MainViewModel final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int counter READ counter NOTIFY counterChanged FINAL)
    Q_PROPERTY(QString pageTitle READ pageTitle CONSTANT FINAL)
    Q_PROPERTY(QVariantList tasks READ tasks NOTIFY tasksChanged FINAL)
    Q_PROPERTY(QString lastDbError READ lastDbError NOTIFY lastDbErrorChanged FINAL)
    Q_PROPERTY(QStringList taskStatusOptions READ taskStatusOptions CONSTANT FINAL)
    Q_PROPERTY(QVariantMap taskFields READ taskFields CONSTANT FINAL)

public:
    /**
     * @brief Creates the root view-model and its child coordinators.
     * @param appModel Application model providing shared counter state.
     * @param taskRepository Repository shared by task list and task action flows.
     * @param parent Owning QObject parent.
     */
    explicit MainViewModel(AppModel *appModel,
                           TaskRepository *taskRepository,
                           QObject *parent = nullptr);

    /// @brief Returns the current counter value from `AppModel`.
    int counter() const;
    /// @brief Returns the title shown by the main page.
    QString pageTitle() const;
    /// @brief Returns the latest task list payload exposed to QML.
    QVariantList tasks() const;
    /// @brief Returns the last database-related error surfaced to the UI.
    QString lastDbError() const;
    /// @brief Returns display labels for all supported task statuses.
    QStringList taskStatusOptions() const;
    /// @brief Returns the field map contract used by QML forms and delegates.
    QVariantMap taskFields() const;

    /// @brief Increments the demo counter owned by `AppModel`.
    Q_INVOKABLE void incrementCounter();
    /// @brief Resets the demo counter to zero.
    Q_INVOKABLE void resetCounter();
    /**
     * @brief Validates and creates a new task through the action view-model.
     * @param name Required task name.
     * @param description Optional task description.
     * @param dueAtIso Due date in ISO8601 format.
     * @param status Numeric value matching `TaskStatus`.
     * @param autoDelay Whether automatic delay handling is enabled.
     * @return `true` when creation and list refresh both succeed.
     */
    Q_INVOKABLE bool create(const QString &name,
                            const QString &description,
                            const QString &dueAtIso,
                            int status = 0,
                            bool autoDelay = false);
    /// @brief Loads the latest tasks from the repository into the QML-facing list.
    Q_INVOKABLE bool loadTasks();
    /// @brief Returns one task payload by identifier.
    Q_INVOKABLE QVariantMap get(qint64 id);
    /// @brief Loads and returns the full task list payload.
    Q_INVOKABLE QVariantList getAll();
    /// @brief Updates a task status and refreshes the exported list.
    Q_INVOKABLE bool updateStatus(qint64 id, int status);
    /// @brief Removes a task and refreshes the exported list.
    Q_INVOKABLE bool remove(qint64 id);

signals:
    void counterChanged();
    void tasksChanged();
    void lastDbErrorChanged();
    void errorOccurred(const QString &message);

private:
    bool runTaskMutation(const std::function<bool()> &mutation);
    void onChildOperationFailed(const QString &operation, const QString &detail, bool notifyUser);

    AppModel *m_appModel = nullptr;
    TaskListViewModel *m_taskListViewModel = nullptr;
    TaskActionViewModel *m_taskActionViewModel = nullptr;
    GlobalStateViewModel *m_globalStateViewModel = nullptr;
};
