/*
 *  SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.12
import org.kde.kirigami 2.18 as Kirigami

/**
 * Component to create CSD dialogs that come from the system.
 */
Kirigami.AbstractApplicationWindow {
    id: root

    title: mainText
    /**
     * Main text of the dialog.
     */
    property string mainText: title

    /**
     * Subtitle of the dialog.
     */
    property string subtitle: ""

    /**
     * This property holds the icon used in the dialog.
     */
    property string iconName: ""

    /**
     * This property holds the list of actions for this dialog.
     *
     * Each action will be rendered as a button that the user will be able
     * to click.
     */
    property list<Kirigami.Action> actions

    default property Item mainItem

    /**
     * This property holds the QQC2 DialogButtonBox used in the footer of the dialog.
     */
    readonly property DialogButtonBox dialogButtonBox: contentDialog.item.dialogButtonBox

    /**
     * Provides dialogButtonBox.standardButtons
     *
     * Useful to be able to set it as dialogButtonBox will be null as the object gets built
     */
    property variant standardButtons: contentDialog.item ? contentDialog.item.dialogButtonBox.standardButtons : undefined

    /**
     * Controls whether the accept button is enabled
     */
    property bool acceptable: true

    property bool showCloseButton: false


    /**
     * The layout of the action buttons in the footer of the dialog.
     *
     * By default, if there are more than 3 actions, it will have `Qt.Vertical`.
     *
     * Otherwise, with zero to 2 actions, it will have `Qt.Horizontal`.
     *
     * This will only affect on Mobile
     */
    property int /*Qt.Orientation*/ layout: actions.length > 3 ? Qt.Vertical : Qt.Horizontal

    flags: contentDialog.item.flags
    width: contentDialog.item.implicitWidth
    height: contentDialog.item.implicitHeight
    visible: false
    minimumWidth: contentDialog.item.minimumWidth
    minimumHeight: contentDialog.item.minimumHeight

    signal accept()
    signal reject()
    property bool accepted: false
    onAccept: {
        accepted = true
        close()
    }
    onReject: close()

    onVisibleChanged: if (!visible && !accepted) {
        root.reject()
    }

    Binding {
        target: dialogButtonBox.standardButton(DialogButtonBox.Ok)
        property: "enabled"
        when: dialogButtonBox.standardButton(DialogButtonBox.Ok)
        value: root.acceptable
    }

    Loader {
        id: contentDialog
        anchors.fill: parent
        sourceComponent: Kirigami.Settings.tabletMode || true ? mobileDialog : desktopDialog

        Keys.onEscapePressed: root.reject()
        focus: true
    }

    Component {
        id: mobileDialog
        MobileSystemDialog {
            window: root
            mainText: root.mainText
            actions: root.actions
            subtitle: root.subtitle
            iconName: root.iconName
            mainItem: root.mainItem
            dialogButtonBox.standardButtons: root.standardButtons
        }
    }
    Component {
        id: desktopDialog
        DesktopSystemDialog {
            window: root
            mainText: root.mainText
            subtitle: root.subtitle
            actions: root.actions
            iconName: root.iconName
            mainItem: root.mainItem
            dialogButtonBox.standardButtons: root.standardButtons
        }
    }
}
