#pragma once

#include <QObject>
#include <QString>

class TaskRepository;

/**
 * @brief Handles task mutations and reports validation or repository failures to the UI.
 */
class TaskActionViewModel final : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates an action view-model bound to a repository instance.
     * @param taskRepository Repository used for create/update/delete operations.
     * @param parent Owning QObject parent.
     */
    explicit TaskActionViewModel(TaskRepository *taskRepository, QObject *parent = nullptr);

    /**
     * @brief Validates form input and creates a new task.
     * @param name Required task name.
     * @param description Optional task description.
     * @param dueAtIso Due date in ISO8601 format.
     * @param status Numeric value matching `TaskStatus`.
     * @param autoDelay Whether automatic delay handling is enabled.
     * @return `true` when repository insertion succeeds.
     */
    bool create(const QString &name,
                const QString &description,
                const QString &dueAtIso,
                int status = 0,
                bool autoDelay = false);
    /**
     * @brief Updates the status of an existing task.
     * @param id Task identifier.
     * @param status Numeric value matching `TaskStatus`.
     * @return `true` when the task exists and the update succeeds.
     */
    bool updateStatus(qint64 id, int status);
    /**
     * @brief Deletes a task by identifier.
     * @param id Task identifier.
     * @return `true` when repository deletion succeeds.
     */
    bool remove(qint64 id);

signals:
    /**
     * @brief Emitted when an action fails validation or persistence.
     * @param operation User-facing operation label.
     * @param detail Detailed failure reason.
     * @param notifyUser Whether the UI should surface the failure immediately.
     */
    void operationFailed(const QString &operation, const QString &detail, bool notifyUser);

private:
    TaskRepository *m_taskRepository = nullptr;
};
