#include "viewmodel/MainViewModel.h"

#include "infrastructure/logging/Logging.h"
#include "model/AppModel.h"

MainViewModel::MainViewModel(AppModel *appModel, QObject *parent)
    : QObject(parent)
    , m_appModel(appModel)
{
    Q_ASSERT(m_appModel != nullptr);
    connect(m_appModel, &AppModel::counterChanged, this, &MainViewModel::counterChanged);
    app::logging::info("MainViewModel initialized");
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
    app::logging::info("Counter incremented to {}", m_appModel->counter());
}

void MainViewModel::resetCounter()
{
    m_appModel->setCounter(0);
    app::logging::info("Counter reset");
}
