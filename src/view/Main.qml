import QtQuick
import QtQuick.Controls

Window {
    width: 960
    height: 680
    visible: true
    title: mainViewModel.pageTitle

    GlobalErrorDialog {
        id: errorDialog
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: TaskPage {
            reportError: function (message) {
                errorDialog.showError(message)
            }
        }
    }
}
