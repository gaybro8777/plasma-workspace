/*
  SPDX-FileCopyrightLabel: 2021 Han Young <hanyoung@protonmail.com>
  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
  SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>
  SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
  SPDX-License-Identifier: LGPL-3.0-or-later
*/
import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.15 as Kirigami
import org.kde.kcm 1.2 as KCM
import LanguageListModel 1.0

KCM.ScrollViewKCM {
    id: languageSelectPage
    title: i18n("Language")
    LanguageListModel {
        id: languageListModel
        Component.onCompleted: {
            languageListModel.selectedLanguageModel.setFormatsSettings(kcm.settings);
        }
    }
    Component {
        id: languagesListItemComponent

        Item {
            width: ListView.view.width
            height: listItem.implicitHeight

            Kirigami.SwipeListItem {
                id: listItem

                contentItem: RowLayout {
                    Kirigami.ListItemDragHandle {
                        listItem: listItem
                        listView: languageListView
                        visible: languageListView.count > 1
                        onMoveRequested: {
                            languageListModel.selectedLanguageModel.move(oldIndex, newIndex);
                        }
                    }

//                    QQC2.BusyIndicator {
//                        visible: model.IsInstalling
//                        running: visible
//                        // the control style (e.g. org.kde.desktop) may force a padding that will shrink the indicator way down. ignore it.
//                        padding: 0

//                        Layout.alignment: Qt.AlignVCenter

//                        implicitWidth: Kirigami.Units.iconSizes.small
//                        implicitHeight: implicitWidth

//                        QQC2.ToolTip {
//                            text: xi18nc('@info:tooltip/rich',
//                                         'Installing missing packages to complete this translation.')
//                        }
//                    }

//                    Kirigami.Icon {
//                        visible: model.IsIncomplete

//                        Layout.alignment: Qt.AlignVCenter

//                        implicitWidth: Kirigami.Units.iconSizes.small
//                        implicitHeight: implicitWidth

//                        source: "data-warning"
//                        color: Kirigami.Theme.negativeTextColor
//                        MouseArea {
//                            id: area
//                            anchors.fill: parent
//                            hoverEnabled: true
//                        }
//                        QQC2.ToolTip {
//                            visible: area.containsMouse
//                            text: xi18nc('@info:tooltip/rich',
//                                         `Not all translations for this language are installed.
//                                          Use the <interface>Install Missing Packages</interface> button to download
//                                          and install all missing packages.`)
//                        }

//                    }

                    QQC2.Label {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter

                        text: switch(index){
                            // Don't assing undefind to string if the index is invalid.
                            case -1: ""; break;
                            case 0: i18nc("@item:inlistbox 1 = Language name", "%1 (Default)", model.display); break;
                            default: model.display; break;
                        }

                        color: (listItem.checked || (listItem.pressed && !listItem.checked && !listItem.sectionDelegate)) ? listItem.activeTextColor : listItem.textColor
                    }
                }

            actions: [
//                Kirigami.Action {
//                    visible: model.IsIncomplete
//                    iconName: "install"
//                    tooltip: i18nc("@info:tooltip", "Install Missing Packages")
//                    onTriggered: model.Object.complete()
//                },
                Kirigami.Action {
                    enabled: index > 0
                    visible: languageListView.count > 1
                    iconName: "go-top"
                    tooltip: i18nc("@info:tooltip", "Promote to default")
                    onTriggered: languageListModel.selectedLanguageModel.move(index, 0)
                },
                Kirigami.Action {
                    iconName: "edit-delete"
                    visible: languageListView.count > 1
                    tooltip: i18nc("@info:tooltip", "Remove")
                    onTriggered: languageListModel.selectedLanguageModel.remove(index);
                }]
            }
        }
    }
    view: ListView {
        id: languageListView
        model: languageListModel.selectedLanguageModel
        delegate: languagesListItemComponent
    }

    Component {
        id: addLanguageItemComponent

        Kirigami.CheckableListItem  {
            id: languageItem

            width: availableLanguagesList.width
            reserveSpaceForIcon: false

            label: model.nativeName
            action: Kirigami.Action {
                onTriggered: {
                    checked = !checked;
                    if (checked) {
                        addLanguagesSheet.selectedLanguages.push(model.languageCode);
                        addLanguagesButton.enabled = true;
                    } else {
                        addLanguagesSheet.selectedLanguages = addLanguagesSheet.selectedLanguages.filter(
                                    (item) => item !== model.languageCode);

                        if (addLanguagesSheet.selectedLanguages.length === 0) {
                            addLanguagesButton.enabled = false;
                        }
                    }
                }
            }
            data: [Connections {
                target: addLanguagesSheet

                function onSheetOpenChanged() {
                    languageItem.checked = false
                }
            }]
        }
    }

    Kirigami.OverlaySheet {
        id: addLanguagesSheet
        property var selectedLanguages: []
        onSheetOpenChanged: selectedLanguages = []

        parent: languageSelectPage

        topPadding: 0
        leftPadding: 0
        rightPadding: 0
        bottomPadding: 0

        title: i18nc("@title:window", "Add Languages")

        ListView {
            id: availableLanguagesList
            implicitWidth: 18 * Kirigami.Units.gridUnit
            model: languageListModel
            delegate: addLanguageItemComponent
        }

        footer: RowLayout {
            QQC2.Button {
                id: addLanguagesButton

                Layout.alignment: Qt.AlignHCenter
                text: i18nc("@action:button", "Add")
                enabled: false
                onClicked: {
                    languageListModel.selectedLanguageModel.addLanguages(addLanguagesSheet.selectedLanguages);
                    addLanguagesSheet.sheetOpen = false;
                }
            }
        }
    }
    footer: RowLayout {
        id: footerLayout

        QQC2.Button {
            Layout.alignment: Qt.AlignRight

            enabled: availableLanguagesList.count

            text: i18nc("@action:button", "Add languages…")

            onClicked: addLanguagesSheet.sheetOpen = !addLanguagesSheet.sheetOpen

            checkable: true
            checked: addLanguagesSheet.sheetOpen
        }
    }
}
