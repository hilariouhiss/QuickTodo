import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    required property var task
    required property int listWidth

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
                text: root.task.name
                font.bold: true
                Layout.fillWidth: true
            }

            ComboBox {
                id: statusCombo
                model: ["未开始", "进行中", "挂起", "已完成"]
                currentIndex: Number(root.task.status)
                onActivated: function (index) {
                    root.statusUpdateRequested(Number(root.task.id), index)
                }
            }

            Button {
                text: "删除"
                onClicked: root.removeRequested(Number(root.task.id))
            }
        }

        Label {
            text: "状态: " + root.task.statusText + "（" + root.task.status + "）"
        }

        Label {
            text: "预计完成时间: " + root.task.dueAt
        }
    }
}
