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
    property alias mainText: contentDialog.mainText

    /**
     * Subtitle of the dialog.
     */
    property alias subtitle: contentDialog.subtitle

    /**
     * This property holds the icon used in the dialog.
     */
    property alias iconName: contentDialog.iconName

    /**
     * This property holds the list of actions for this dialog.
     *
     * Each action will be rendered as a button that the user will be able
     * to click.
     */
    property alias actions: contentDialog.actions

    default property alias mainItem: contentDialog.mainItem

    /**
     * This property holds the QQC2 DialogButtonBox used in the footer of the dialog.
     */
    readonly property alias dialogButtonBox: contentDialog.dialogButtonBox

    /**
     * Controls whether the accept button is enabled
     */
    property bool acceptable: true

    flags: contentDialog.flags
    width: contentDialog.implicitWidth
    height: contentDialog.implicitHeight
    visible: false
    minimumWidth: contentDialog.minimumWidth
    minimumHeight: contentDialog.minimumHeight

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

    MobileSystemDialog {
        id: contentDialog
        anchors.fill: parent

        window: root
        mainText: root.title

        Keys.onEscapePressed: root.reject()
    }
}
