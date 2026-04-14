import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property var reportError
    property var tasks: []

    function notifyError(fallbackText) {
        const dbError = mainViewModel.lastDbError
        const message = dbError !== "" ? dbError : fallbackText
        if (message === "") {
            return
        }
        if (typeof root.reportError === "function") {
            root.reportError(message)
        }
    }

    function reloadTasks() {
        root.tasks = mainViewModel.getAll()
        if (mainViewModel.lastDbError !== "") {
            notifyError("加载任务失败")
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            text: mainViewModel.pageTitle
            font.pixelSize: 24
            font.bold: true
            Layout.fillWidth: true
        }

        TaskList {
            Layout.fillWidth: true
            Layout.fillHeight: true
            tasks: root.tasks
            onStatusUpdateRequested: function (taskId, status) {
                const ok = mainViewModel.updateStatus(taskId, status)
                if (!ok) {
                    notifyError("更新状态失败")
                    return
                }
                reloadTasks()
            }
            onRemoveRequested: function (taskId) {
                const ok = mainViewModel.remove(taskId)
                if (!ok) {
                    notifyError("删除任务失败")
                    return
                }
                reloadTasks()
            }
        }
    }

    Component.onCompleted: reloadTasks()
}
