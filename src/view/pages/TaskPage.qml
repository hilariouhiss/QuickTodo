import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

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
            tasks: mainViewModel.tasks
            onStatusUpdateRequested: function (taskId, status) {
                mainViewModel.updateStatus(taskId, status)
            }
            onRemoveRequested: function (taskId) {
                mainViewModel.remove(taskId)
            }
        }
    }

    Component.onCompleted: {
        mainViewModel.loadTasks()
    }
}
