#pragma once

#include <QObject>
#include <QString>

class GlobalStateViewModel final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged FINAL)

public:
    explicit GlobalStateViewModel(QObject *parent = nullptr);

    QString lastError() const;
    void clearError();
    void setErrorMessage(const QString &message, bool notifyUser = true);
    void reportOperationError(const QString &operation,
                              const QString &detail = QString(),
                              bool notifyUser = true);

signals:
    void lastErrorChanged();
    void errorOccurred(const QString &message);

private:
    QString m_lastError;
};
