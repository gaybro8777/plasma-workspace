/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Dialogs 1.2 as Dialogs
import QtGraphicalEffects 1.12
import org.kde.kirigami 2.19 as Kirigami
import org.kde.plasma.workspace.dialogs 1.0

Kirigami.AbstractApplicationWindow {
    id: root
    
    width: 600
    height: 600

    SystemDialog {
        id: simple
        mainText: "Reset Data"
        subtitle: "This will reset all of your data."
        iconName: "documentinfo"
        
        dialogButtonBox.standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
    }
    
    SystemDialog {
        id: simpleCSD
        mainText: "Reset Data"
        subtitle: "This will reset all of your data."
        iconName: "documentinfo"
        
        width: Kirigami.Units.gridUnit * 17
        Component.onCompleted: {
            dialogButtonBox.standardButtons = DialogButtonBox.Ok | DialogButtonBox.Cancel
        }
    }
    
    SystemDialog {
        id: desktopPolkit
        mainText: "Authentication Required"
        subtitle: "Authentication is needed to run `/usr/bin/ls` as the super user."
        iconName: "im-user-online"

        Kirigami.PasswordField {}

        Component.onCompleted: {
            dialogButtonBox.standardButtons = DialogButtonBox.Ok | DialogButtonBox.Cancel
        }
        actions: [
            Kirigami.Action {
                text: "Details"
                iconName: "documentinfo"
                onTriggered: desktopPolkit.close()
            }
        ]
    }

    SystemDialog {
        id: xdgDialog
        mainText: "Wallet access"
        subtitle: "Share your wallet with 'Somebody'."
        iconName: "kwallet"
        acceptable: false

        Component.onCompleted: {
            dialogButtonBox.standardButtons = DialogButtonBox.Ok | DialogButtonBox.Cancel
            dialogButtonBox.standardButton(DialogButtonBox.Ok).text = "Share"
        }
        actions: [
            Kirigami.Action {
                text: "Something Happens"
                iconName: "documentinfo"
                onTriggered: xdgDialog.acceptable = true
            }
        ]
    }
    
    SystemDialog {
        id: desktopCSDPolkit
        mainText: "Authentication Required"
        subtitle: "Authentication is needed to run `/usr/bin/ls` as the super user."
        iconName: "im-user-online"
        width: Kirigami.Units.gridUnit * 25
        
        Kirigami.PasswordField {}
        
        Component.onCompleted: {
            dialogButtonBox.standardButtons = DialogButtonBox.Ok | DialogButtonBox.Cancel
        }
        actions: [
            Kirigami.Action {
                text: "Details"
                iconName: "documentinfo"
                onTriggered: desktopCSDPolkit.close()
            }
        ]
    }
    
    SystemDialog {
        id: mobilePolkit
        mainText: "Authentication Required"
        subtitle: "Authentication is needed to run `/usr/bin/ls` as the super user."

        ColumnLayout {
            width: Kirigami.Units.gridUnit * 20

            Kirigami.Avatar {
                implicitHeight: Kirigami.Units.iconSizes.medium
                implicitWidth: Kirigami.Units.iconSizes.medium
                Layout.alignment: Qt.AlignHCenter
            }
            Kirigami.PasswordField {
                Layout.fillWidth: true
            }
        }
        
        Component.onCompleted: {
            dialogButtonBox.standardButtons = DialogButtonBox.Ok | DialogButtonBox.Cancel
        }
        actions: [
            Kirigami.Action {
                text: "Details"
                iconName: "documentinfo"
                onTriggered: mobilePolkit.close()
            }
        ]
    }
    
    SystemDialog {
        id: sim
        mainText: "SIM Locked"
        subtitle: "Please enter your SIM PIN in order to unlock it."
        
        width: Kirigami.Units.gridUnit * 20
        
        Kirigami.PasswordField {
            Layout.fillWidth: true
        }
        
        Component.onCompleted: {
            dialogButtonBox.standardButtons = DialogButtonBox.Ok | DialogButtonBox.Cancel
        }
    }
    
    SystemDialog {
        id: device
        mainText: "Device Request"
        subtitle: "Allow <b>PureMaps</b> to access your location?"

        actions: [
            Kirigami.Action {
                text: "Allow all the time"
                onTriggered: device.accept()
            },
            Kirigami.Action {
                text: "Allow only while the app is in use"
                onTriggered: device.accept()
            },
            Kirigami.Action {
                text: "Deny"
                onTriggered: device.accept()
            }
        ]
    }
    
    SystemDialog {
        id: wifi
        mainText: "eduroam"
        width: Kirigami.Units.gridUnit * 18
        maximumHeight: Kirigami.Units.gridUnit * 20
        
        Kirigami.FormLayout {
            ComboBox {
                model: ["PEAP"]
                Layout.fillWidth: true
                Kirigami.FormData.label: "EAP method:"
                currentIndex: 0
            }
            ComboBox {
                model: ["MSCHAPV2"]
                Layout.fillWidth: true
                Kirigami.FormData.label: "Phase 2 authentication:"
                currentIndex: 0
            }
            TextField {
                Kirigami.FormData.label: "Domain:"
                Layout.fillWidth: true
                text: ""
            }
            TextField {
                Kirigami.FormData.label: "Identity:"
                Layout.fillWidth: true
            }
            TextField {
                Kirigami.FormData.label: "Username:"
                Layout.fillWidth: true
            }
            Kirigami.PasswordField {
                Kirigami.FormData.label: "Password:"
                Layout.fillWidth: true
            }
        }

        Component.onCompleted: {
            dialogButtonBox.standardButtons = DialogButtonBox.Ok | DialogButtonBox.Cancel
            dialogButtonBox.standardButton(DialogButtonBox.Ok).text = "Save"
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        CheckBox {
            id: checkbox
            text: "Fullscreen"
        }
        Button {
            text: "Simple dialog (Desktop)"
            onClicked: {
                if (checkbox.checked) {
                    simple.showFullScreen()
                } else {
                    simple.show()
                }
            }
        }
        Button {
            text: "Simple dialog (Desktop CSD)"
            onClicked: {
                if (checkbox.checked) {
                    simpleCSD.showFullScreen()
                } else {
                    simpleCSD.show()
                }
            }
        }
        Button {
            text: "Polkit dialog (Desktop)"
            onClicked: {
                if (checkbox.checked) {
                    desktopPolkit.showFullScreen()
                } else {
                    desktopPolkit.show()
                }
            }
        }
        Button {
            text: "Polkit dialog (Desktop CSD)"
            onClicked: {
                if (checkbox.checked) {
                    desktopCSDPolkit.showFullScreen()
                } else {
                    desktopCSDPolkit.show()
                }
            }
        }
        Button {
            text: "XDG dialog (Desktop)"
            onClicked: {
                if (checkbox.checked) {
                    xdgDialog.showFullScreen()
                } else {
                    xdgDialog.show()
                }
            }
        }
        Button {
            text: "Polkit dialog (Mobile)"
            onClicked: {
                if (checkbox.checked) {
                    mobilePolkit.showFullScreen()
                } else {
                    mobilePolkit.show()
                }
            }
        }
        Button {
            text: "SIM PIN dialog (Mobile)"
            onClicked: {
                if (checkbox.checked) {
                    sim.showFullScreen()
                } else {
                    sim.show()
                }
            }
        }
        Button {
            text: "Device request dialog (Mobile)"
            onClicked: {
                if (checkbox.checked) {
                    device.showFullScreen()
                } else {
                    device.show()
                }
            }
        }
        Button {
            text: "Wifi Dialog (Mobile)"
            onClicked: {
                if (checkbox.checked) {
                    wifi.showFullScreen()
                } else {
                    wifi.show()
                }
            }
        }
    }
} 

