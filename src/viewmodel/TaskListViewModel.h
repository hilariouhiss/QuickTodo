#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class Task;
class TaskRepository;

class TaskListViewModel final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList tasks READ tasks NOTIFY tasksChanged FINAL)

public:
    explicit TaskListViewModel(TaskRepository *taskRepository, QObject *parent = nullptr);

    QVariantList tasks() const;
    bool loadTasks();
    bool reloadAfterMutation();
    QVariantMap get(qint64 id);
    QVariantList getAll();

signals:
    void tasksChanged();
    void operationFailed(const QString &operation, const QString &detail, bool notifyUser);

private:
    void setTasks(const QVariantList &tasks);
    QVariantMap taskToMap(const Task &task) const;

private:
    TaskRepository *m_taskRepository = nullptr;
    QVariantList m_tasks;
};
