#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class Task;
class TaskRepository;

/**
 * @brief Exposes repository task data as QVariant payloads suitable for QML binding.
 */
class TaskListViewModel final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList tasks READ tasks NOTIFY tasksChanged FINAL)

public:
    /**
     * @brief Creates a list view-model bound to a repository instance.
     * @param taskRepository Repository used for read operations.
     * @param parent Owning QObject parent.
     */
    explicit TaskListViewModel(TaskRepository *taskRepository, QObject *parent = nullptr);

    /// @brief Returns the current QML-facing task list payload.
    QVariantList tasks() const;
    /// @brief Loads the latest task snapshot from the repository.
    bool loadTasks();
    /// @brief Reloads tasks after a successful mutation.
    bool reloadAfterMutation();
    /// @brief Returns one task payload by identifier.
    QVariantMap get(qint64 id);
    /// @brief Loads and returns the full task list payload.
    QVariantList getAll();

signals:
    /// @brief Emitted when the exported task list changes.
    void tasksChanged();
    /**
     * @brief Emitted when a read operation fails.
     * @param operation User-facing operation label.
     * @param detail Detailed failure reason.
     * @param notifyUser Whether the UI should surface the failure immediately.
     */
    void operationFailed(const QString &operation, const QString &detail, bool notifyUser);

private:
    void setTasks(const QVariantList &tasks);
    QVariantMap taskToMap(const Task &task) const;

private:
    TaskRepository *m_taskRepository = nullptr;
    QVariantList m_tasks;
};
