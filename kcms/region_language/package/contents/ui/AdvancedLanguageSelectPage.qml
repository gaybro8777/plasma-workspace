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
import kcmregionandlang 1.0

KCM.ScrollViewKCM {
    id: languageSelectPage
    title: i18n("Language")
    LanguageListModel {
        id: languageListModel
        Component.onCompleted: {
            languageListModel.setRegionAndLangSettings(kcm.settings);
        }
    }
    header: ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        Kirigami.InlineMessage {
            text: i18n("Adding more than two languages will have undesired behavior on some applications")
            Layout.fillWidth: true
            type: Kirigami.MessageType.Warning
            visible: languageListView.count > 1
        }

        QQC2.Label {
            horizontalAlignment: Qt.AlignHCenter
            text: i18n("The first language will also be used for formats settings \n languages on the top will be used first if Apps support it")
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

                    QQC2.Label {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter

                        text: model.display

                        color: listItem.checked || (listItem.pressed && !listItem.checked && !listItem.sectionDelegate) ? listItem.activeTextColor : listItem.textColor;
                    }
                }

                actions: [
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
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("There is no language config at Plasma level")
        }
    }

    Component {
        id: addLanguageItemComponent

        Kirigami.BasicListItem  {
            id: languageItem

            width: availableLanguagesList.width
            reserveSpaceForIcon: false

            label: model.nativeName
            action: Kirigami.Action {
                onTriggered: {
                    languageListModel.selectedLanguageModel.addLanguage(model.languageCode);
                    addLanguagesSheet.close();
                }
            }
        }
    }

    Kirigami.OverlaySheet {
        id: addLanguagesSheet
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
    }
    footer: ColumnLayout {
        QQC2.Button {
            Layout.alignment: Qt.AlignRight
            enabled: availableLanguagesList.count

            text: i18nc("@action:button", "Add languages…")

            onClicked: addLanguagesSheet.sheetOpen = !addLanguagesSheet.sheetOpen

            checkable: true
            checked: addLanguagesSheet.sheetOpen
        }
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            QQC2.Label {
                text: languageListModel.numberExample
            }
            QQC2.Label {
                text: languageListModel.currencyExample
            }
        }

        QQC2.Label {
            Layout.alignment: Qt.AlignHCenter
            text: languageListModel.metric
        }

        QQC2.Label {
            Layout.alignment: Qt.AlignHCenter
            text: languageListModel.timeExample
        }
    }
}
