#include "model/AppModel.h"

AppModel::AppModel(QObject *parent)
    : QObject(parent)
{}

int AppModel::counter() const
{
    return m_counter;
}

void AppModel::setCounter(int value)
{
    if (m_counter == value) {
        return;
    }
    m_counter = value;
    emit counterChanged();
}