import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var vm: mainViewModel

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            text: vm.pageTitle
            font.pixelSize: 24
            font.bold: true
            Layout.fillWidth: true
        }

        TaskList {
            Layout.fillWidth: true
            Layout.fillHeight: true
            tasks: vm.tasks
            onStatusUpdateRequested: function (taskId, status) {
                vm.updateStatus(taskId, status)
            }
            onRemoveRequested: function (taskId) {
                vm.remove(taskId)
            }
        }
    }

    Component.onCompleted: {
        vm.loadTasks()
    }
}
