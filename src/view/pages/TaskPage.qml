import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    // The page only binds UI to the shared VM and forwards user actions without owning business logic.
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
            statusOptions: vm.taskStatusOptions
            taskFields: vm.taskFields
            onStatusUpdateRequested: function (taskId, status) {
                vm.updateStatus(taskId, status)
            }
            onRemoveRequested: function (taskId) {
                vm.remove(taskId)
            }
        }
    }

    Component.onCompleted: {
        // Trigger the initial refresh when the page is first instantiated.
        vm.loadTasks()
    }
}
