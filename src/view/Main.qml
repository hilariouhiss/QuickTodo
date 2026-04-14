import QtQuick
import QtQuick.Controls

Window {
    readonly property var vm: mainViewModel

    width: 960
    height: 680
    visible: true
    title: vm.pageTitle

    GlobalErrorDialog {
        id: errorDialog
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: TaskPage {}
    }

    Connections {
        target: vm

        function onErrorOccurred(message) {
            errorDialog.showError(message)
        }
    }
}
