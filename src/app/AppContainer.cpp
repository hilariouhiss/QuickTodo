#include "app/AppContainer.h"

AppContainer::AppContainer()
    : m_appModel(nullptr)
    , m_mainViewModel(&m_appModel, nullptr)
{}

MainViewModel *AppContainer::mainViewModel()
{
    return &m_mainViewModel;
}