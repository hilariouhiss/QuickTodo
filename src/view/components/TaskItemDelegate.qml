import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    required property var task
    required property int listWidth
    required property var statusOptions
    // Field names are supplied by the viewmodel so the delegate stays agnostic to backend key changes.
    required property var taskFields

    signal statusUpdateRequested(int taskId, int status)

    signal removeRequested(int taskId)

    width: listWidth
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
                text: root.task[root.taskFields.name]
                font.bold: true
                Layout.fillWidth: true
            }

            ComboBox {
                id: statusCombo
                model: root.statusOptions
                // The option order is expected to stay aligned with the numeric status enum exposed by the VM.
                currentIndex: Number(root.task[root.taskFields.status])
                onActivated: function (index) {
                    root.statusUpdateRequested(Number(root.task[root.taskFields.id]), index)
                }
            }

            Button {
                text: "删除"
                onClicked: root.removeRequested(Number(root.task[root.taskFields.id]))
            }
        }

        Label {
            text: "状态: " + root.task[root.taskFields.statusText] + "（" + root.task[root.taskFields.status] + "）"
        }

        Label {
            text: "预计完成时间: " + root.task[root.taskFields.dueAt]
        }
    }
}
