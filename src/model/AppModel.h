#pragma once

#include <QObject>

class AppModel final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int counter READ counter WRITE setCounter NOTIFY counterChanged FINAL)

public:
    explicit AppModel(QObject *parent = nullptr);

    int counter() const;
    void setCounter(int value);

signals:
    void counterChanged();

private:
    int m_counter = 0;
};
