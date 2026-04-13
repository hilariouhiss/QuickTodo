#pragma once

#include "model/AppModel.h"
#include "viewmodel/MainViewModel.h"

class AppContainer final
{
public:
    AppContainer();

    MainViewModel *mainViewModel();

private:
    AppModel m_appModel;
    MainViewModel m_mainViewModel;
};