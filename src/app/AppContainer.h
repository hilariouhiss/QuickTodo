#pragma once

#include "infrastructure/database/DatabaseManager.h"
#include "model/AppModel.h"
#include "model/repository/TaskRepository.h"
#include "viewmodel/MainViewModel.h"

class AppContainer final
{
public:
    AppContainer();

    MainViewModel *mainViewModel();
    bool databaseReady() const;
    QString databaseError() const;

private:
    AppModel m_appModel;
    DatabaseManager m_databaseManager;
    TaskRepository m_taskRepository;
    MainViewModel m_mainViewModel;
    bool m_databaseReady = false;
    QString m_databaseError;
};
