import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Window {
    width: 960
    height: 680
    visible: true
    title: mainViewModel.pageTitle

    property var tasks: []

    function showErrorIfNeeded(fallbackText) {
        if (mainViewModel.lastDbError !== "") {
            errorDialog.text = mainViewModel.lastDbError
            errorDialog.open()
            return
        }
        if (fallbackText !== "") {
            errorDialog.text = fallbackText
            errorDialog.open()
        }
    }

    function reloadTasks() {
        tasks = mainViewModel.getAll()
        if (mainViewModel.lastDbError !== "") {
            showErrorIfNeeded("加载任务失败")
        }
    }

    MessageDialog {
        id: errorDialog
        title: "操作失败"
        text: ""
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: mainViewModel.pageTitle
                font.pixelSize: 24
                font.bold: true
                Layout.fillWidth: true
            }

            Button {
                text: "刷新"
                onClicked: reloadTasks()
            }
        }

        Label {
            visible: tasks.length === 0
            text: "暂无任务"
            color: "#666666"
        }

        ListView {
            id: taskListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: tasks
            spacing: 8
            clip: true

            delegate: Rectangle {
                required property var modelData
                property var task: modelData

                width: taskListView.width
                radius: 8
                color: "#ffffff"
                border.color: "#dddddd"
                implicitHeight: taskColumn.implicitHeight + 20

                ColumnLayout {
                    id: taskColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 10
                    spacing: 6

                    RowLayout {
                        Layout.fillWidth: true

                        Label {
                            text: task.name
                            font.bold: true
                            Layout.fillWidth: true
                        }

                        ComboBox {
                            id: rowStatusCombo
                            model: ["未开始", "进行中", "挂起", "已完成"]
                            currentIndex: Number(task.status)
                            onActivated: function (index) {
                                const ok = mainViewModel.updateStatus(Number(task.id), index)
                                if (!ok) {
                                    showErrorIfNeeded("更新状态失败")
                                    rowStatusCombo.currentIndex = Number(task.status)
                                    return
                                }
                                reloadTasks()
                            }
                        }

                        Button {
                            text: "删除"
                            onClicked: {
                                const ok = mainViewModel.remove(Number(task.id))
                                if (!ok) {
                                    showErrorIfNeeded("删除任务失败")
                                    return
                                }
                                reloadTasks()
                            }
                        }
                    }

                    Label {
                        text: "状态: " + task.statusText + "（" + task.status + "）"
                    }

                    Label {
                        text: "预计完成时间: " + task.dueAt
                    }
                }
            }
        }
    }

    Component.onCompleted: reloadTasks()
}
