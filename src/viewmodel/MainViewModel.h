#pragma once

#include <QObject>
#include <QString>

class AppModel;

class MainViewModel final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int counter READ counter NOTIFY counterChanged FINAL)
    Q_PROPERTY(QString pageTitle READ pageTitle CONSTANT FINAL)

public:
    explicit MainViewModel(AppModel *appModel, QObject *parent = nullptr);

    int counter() const;
    QString pageTitle() const;

    Q_INVOKABLE void incrementCounter();
    Q_INVOKABLE void resetCounter();

signals:
    void counterChanged();

private:
    AppModel *m_appModel = nullptr;
};
