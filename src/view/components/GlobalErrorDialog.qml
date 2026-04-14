import QtQuick
import QtQuick.Dialogs

MessageDialog {
    id: root

    title: "操作失败"
    text: ""

    function showError(message) {
        if (message === undefined || message === null || message === "") {
            return
        }
        root.text = String(message)
        root.open()
    }
}
