# Quick Todo

## 开发入口
- 应用装配入口：`main.cpp`
- 应用容器：`src/app/AppContainer.*`
- 状态模型：`src/model/AppModel.*`
- 主视图模型编排层：`src/viewmodel/MainViewModel.*`
- 子视图模型：
  - `src/viewmodel/TaskListViewModel.*`
  - `src/viewmodel/TaskActionViewModel.*`
  - `src/viewmodel/GlobalStateViewModel.*`
- 界面视图：`Main.qml`

## MVVM 约定
- `Main.qml` 只做展示和交互绑定，不承载业务逻辑，并通过全局弹窗展示 ViewModel 错误事件。
- `AppModel` 负责业务状态（`QObject + Q_PROPERTY`）。
- `MainViewModel` 负责子VM编排与中介路由，对外提供统一绑定入口（`tasks`、`errorOccurred`、既有命令）。
- `TaskListViewModel` 负责列表加载与列表状态维护。
- `TaskActionViewModel` 负责创建/更新状态/删除等写操作。
- `GlobalStateViewModel` 负责统一错误收口与全局错误输出。
- 任务字段 key 与状态选项由 C++ 统一导出，QML 仅消费 `taskFields`/`taskStatusOptions`，避免双源维护。
- `main.cpp` 负责生命周期与依赖注入（`ContextProperty: mainViewModel`）。
- 数据库 schema 校验在 `DatabaseManager::initializeDefaultDatabase()` 内单一路径完成，容器层只消费结果。

## 后续扩展
- 新业务先判断读写职责：
  - 读模型优先落在 `TaskListViewModel` 同类子VM；
  - 写命令优先落在 `TaskActionViewModel` 同类子VM；
  - 全局状态/错误优先落在 `GlobalStateViewModel`。
- 仅在需要跨子VM协同时，在 `MainViewModel` 添加中介路由。
- 最后在 `Main.qml` 增加绑定与交互。
