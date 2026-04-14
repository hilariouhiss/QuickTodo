import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property var tasks: []
    property var statusOptions: []
    property var taskFields: ({})

    signal statusUpdateRequested(int taskId, int status)

    signal removeRequested(int taskId)

    Label {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        visible: root.tasks.length === 0
        text: "暂无任务"
        color: "#666666"
    }

    ListView {
        id: taskListView
        anchors.fill: parent
        anchors.topMargin: root.tasks.length === 0 ? 24 : 0
        model: root.tasks
        spacing: 8
        clip: true

        delegate: TaskItemDelegate {
            required property var modelData
            task: modelData
            listWidth: taskListView.width
            statusOptions: root.statusOptions
            taskFields: root.taskFields
            onStatusUpdateRequested: function (taskId, status) {
                root.statusUpdateRequested(taskId, status)
            }
            onRemoveRequested: function (taskId) {
                root.removeRequested(taskId)
            }
        }
    }
}
