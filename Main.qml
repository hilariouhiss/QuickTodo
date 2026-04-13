import QtQuick
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    title: mainViewModel.pageTitle

    Column {
        anchors.centerIn: parent
        spacing: 12

        Text {
            text: qsTr("Counter: %1").arg(mainViewModel.counter)
            font.pixelSize: 24
        }

        Row {
            spacing: 8

            Button {
                text: qsTr("Increment")
                onClicked: mainViewModel.incrementCounter()
            }

            Button {
                text: qsTr("Reset")
                enabled: mainViewModel.counter > 0
                onClicked: mainViewModel.resetCounter()
            }
        }
    }
}
