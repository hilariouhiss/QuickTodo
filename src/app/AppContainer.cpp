#include "app/AppContainer.h"

#include "infrastructure/logging/Logging.h"

AppContainer::AppContainer()
    : m_appModel(nullptr)
    , m_mainViewModel(&m_appModel, nullptr)
{
    app::logging::info("AppContainer initialized");
}

MainViewModel *AppContainer::mainViewModel()
{
    return &m_mainViewModel;
}
