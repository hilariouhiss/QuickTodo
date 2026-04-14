#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class AppModel;
class TaskRepository;

class MainViewModel final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int counter READ counter NOTIFY counterChanged FINAL)
    Q_PROPERTY(QString pageTitle READ pageTitle CONSTANT FINAL)
    Q_PROPERTY(QVariantList tasks READ tasks NOTIFY tasksChanged FINAL)
    Q_PROPERTY(QString lastDbError READ lastDbError NOTIFY lastDbErrorChanged FINAL)

public:
    explicit MainViewModel(AppModel *appModel,
                           TaskRepository *taskRepository,
                           QObject *parent = nullptr);

    int counter() const;
    QString pageTitle() const;
    QVariantList tasks() const;
    QString lastDbError() const;

    Q_INVOKABLE void incrementCounter();
    Q_INVOKABLE void resetCounter();
    Q_INVOKABLE bool create(const QString &name,
                            const QString &description,
                            const QString &dueAtIso,
                            int status = 0,
                            bool autoDelay = false);
    Q_INVOKABLE bool loadTasks();
    Q_INVOKABLE QVariantMap get(qint64 id);
    Q_INVOKABLE QVariantList getAll();
    Q_INVOKABLE bool updateStatus(qint64 id, int status);
    Q_INVOKABLE bool remove(qint64 id);

signals:
    void counterChanged();
    void tasksChanged();
    void lastDbErrorChanged();
    void errorOccurred(const QString &message);

private:
    void setLastDbError(const QString &value);
    bool reportOperationError(const QString &operation, const QString &detail = QString());
    void setTasks(const QVariantList &tasks);
    QVariantMap taskToMap(const class Task &task) const;

    AppModel *m_appModel = nullptr;
    TaskRepository *m_taskRepository = nullptr;
    QVariantList m_tasks;
    QString m_lastDbError;
};
