# Quick Todo

## 开发入口
- 应用装配入口：`main.cpp`
- 应用容器：`src/app/AppContainer.*`
- 状态模型：`src/model/AppModel.*`
- 视图模型：`src/viewmodel/MainViewModel.*`
- 界面视图：`Main.qml`

## MVVM 约定
- `Main.qml` 只做展示和交互绑定，不承载业务逻辑，并通过全局弹窗展示 ViewModel 错误事件。
- `AppModel` 负责业务状态（`QObject + Q_PROPERTY`）。
- `MainViewModel` 负责命令与状态映射（`Q_INVOKABLE` + 可观察属性），并维护任务列表状态（`tasks`）与统一错误输出。
- `main.cpp` 负责生命周期与依赖注入（`ContextProperty: mainViewModel`）。

## 后续扩展
- 新业务优先在 `AppModel` 增加状态与变更信号。
- 在 `MainViewModel` 添加命令与属性映射。
- 最后在 `Main.qml` 增加绑定与交互。
