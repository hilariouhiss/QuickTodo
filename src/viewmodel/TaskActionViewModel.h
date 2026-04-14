#pragma once

#include <QObject>
#include <QString>

class TaskRepository;

class TaskActionViewModel final : public QObject
{
    Q_OBJECT

public:
    explicit TaskActionViewModel(TaskRepository *taskRepository, QObject *parent = nullptr);

    bool create(const QString &name,
                const QString &description,
                const QString &dueAtIso,
                int status = 0,
                bool autoDelay = false);
    bool updateStatus(qint64 id, int status);
    bool remove(qint64 id);

signals:
    void operationFailed(const QString &operation, const QString &detail, bool notifyUser);

private:
    TaskRepository *m_taskRepository = nullptr;
};
