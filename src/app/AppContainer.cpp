#include "app/AppContainer.h"

#include "infrastructure/logging/Logging.h"

AppContainer::AppContainer()
    : m_appModel(nullptr)
    , m_taskRepository(QStringLiteral("main"))
    , m_mainViewModel(&m_appModel, &m_taskRepository, nullptr)
{
    m_databaseReady = m_databaseManager.initializeDefaultDatabase();
    if (!m_databaseReady) {
        m_databaseError = m_databaseManager.lastError();
        logging::error("Database bootstrap failed: {}", m_databaseError.toStdString());
    } else {
        m_databaseError.clear();
        logging::info("Database bootstrap and validation completed");
    }

    logging::info("MainViewModel orchestrates child view models");
    logging::info("AppContainer initialized");
}

MainViewModel *AppContainer::mainViewModel()
{
    return &m_mainViewModel;
}

bool AppContainer::databaseReady() const
{
    return m_databaseReady;
}

QString AppContainer::databaseError() const
{
    return m_databaseError;
}
