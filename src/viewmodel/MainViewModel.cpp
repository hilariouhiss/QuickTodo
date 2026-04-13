#include "viewmodel/MainViewModel.h"

#include "model/AppModel.h"

MainViewModel::MainViewModel(AppModel *appModel, QObject *parent)
    : QObject(parent)
    , m_appModel(appModel)
{
    Q_ASSERT(m_appModel != nullptr);
    connect(m_appModel, &AppModel::counterChanged, this, &MainViewModel::counterChanged);
}

int MainViewModel::counter() const
{
    return m_appModel->counter();
}

QString MainViewModel::pageTitle() const
{
    return QStringLiteral("Underlying Test Platform");
}

void MainViewModel::incrementCounter()
{
    m_appModel->setCounter(m_appModel->counter() + 1);
}

void MainViewModel::resetCounter()
{
    m_appModel->setCounter(0);
}
