#include "viewmodel/GlobalStateViewModel.h"

GlobalStateViewModel::GlobalStateViewModel(QObject *parent)
    : QObject(parent)
{}

QString GlobalStateViewModel::lastError() const
{
    return m_lastError;
}

void GlobalStateViewModel::clearError()
{
    setErrorMessage(QString(), false);
}

void GlobalStateViewModel::setErrorMessage(const QString &message, const bool notifyUser)
{
    const bool changed = m_lastError != message;
    m_lastError = message;
    if (changed) {
        emit lastErrorChanged();
    }
    if (notifyUser && !message.trimmed().isEmpty()) {
        emit errorOccurred(message);
    }
}

void GlobalStateViewModel::reportOperationError(const QString &operation,
                                                const QString &detail,
                                                const bool notifyUser)
{
    QString message = operation;
    if (!detail.trimmed().isEmpty()) {
        message = QStringLiteral("%1：%2").arg(operation, detail);
    }
    setErrorMessage(message, notifyUser);
}
