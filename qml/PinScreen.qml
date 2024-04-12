import QtQuick
import QtQuick.Controls

import sgy.pine.modem

Item {
    id: pinScreenRoot
    anchors.fill: parent

    Dialog {
        id: badPinDialog
        title: qsTr("The entered PIN code is incorrect! Try again")
        standardButtons: Dialog.Ok
        anchors.centerIn: parent
    }

    PinInfo {
        id: pinInfo
        onRemainingTriesChanged: {
            remainingTriesText.text = qsTr("Enter the PIN code to unlock the SIM card. %2 tries left.".arg(remainingTries))
        }
    }

    Button {
        id: quitButton
        text: qsTr("Go away")
        onClicked: {
            Qt.exit(0)
        }
        focusPolicy: Qt.NoFocus
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        font.pixelSize: 20
    }

    Rectangle {
        id: filler
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: quitButton.bottom
        height: (parent.height / 3) - quitButton.height
        Text {
            id: remainingTriesText
            text: qsTr("Enter the PIN code to unlock the SIM card. %2 tries left.".arg(pinInfo.remainingTries))
            anchors.left: filler.left
            anchors.right: filler.right
            anchors.bottom: filler.bottom
            font.pixelSize: 20
            color: "black"
            wrapMode: Text.WordWrap
        }
    }

    TextField {
        id: pinInput
        anchors.top: filler.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        rightPadding: 15
        leftPadding: 15
        height: 40
        readOnly: true
        font.bold: true
        font.pixelSize: 20
        echoMode: showPasswordImg.opacity === 0.5 ? TextInput.Password : TextInput.Normal
        activeFocusOnPress: false
        Image {
            id: showPasswordImg
            source: "res/show-password.png"
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            opacity: 0.5
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    parent.opacity = parent.opacity === 0.5 ? 1.0 : 0.5
                }
            }
        }
    }

    Grid {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: pinInput.bottom
        anchors.bottom: parent.bottom
        columns: 3
        columnSpacing: 5
        rowSpacing: 5
        Repeater {
            model: 12
            delegate: Button {
                required property int index
                height: ((parent.height - pinInput.height) / 4)
                width: (parent.width / 3)
                text: getButtonText(index)
                font.bold: true
                font.pixelSize: 20
                focusPolicy: Qt.NoFocus
                onClicked: {
                    if (text === "<") {
                        pinInput.text = deleteLastChar(pinInput.text)
                    } else if (text === "OK") {
                        var pinSuccess = pinInfo.enterPin(pinInput.text)
                        if (pinSuccess){
                            Qt.exit(0)
                        } else {
                            badPinDialog.open()
                        }
                    } else {
                        pinInput.text = pinInput.text + text;
                    }
                }
            }
        }
    }

    function getButtonText(index){
        switch(index){
            case 9:
                return "<"
            case 10:
                return 0
            case 11:
                return "OK"
            default:
                return index + 1
        }
    }

    function deleteLastChar(str){
        if (str.length === 0){
            return str;
        }
        return str.substr(0, str.length - 1);
    }
}
