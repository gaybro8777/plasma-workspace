/*
  SPDX-FileCopyrightLabel: 2021 Han Young <hanyoung@protonmail.com>

  SPDX-License-Identifier: LGPL-3.0-or-later
*/
import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.15 as Kirigami
import org.kde.kcm 1.2 as KCM
import kcmregionandlang 1.0

KCM.ScrollViewKCM {
    property bool langPageSelected: false

    id: root
    implicitHeight: Kirigami.Units.gridUnit * 40
    implicitWidth: Kirigami.Units.gridUnit * 20
    header: ColumnLayout {
        id: messagesLayout

        spacing: Kirigami.Units.largeSpacing

        Kirigami.InlineMessage {
            id: takeEffectNextTimeMsg
            Layout.fillWidth: true

            type: Kirigami.MessageType.Information

            text: i18nc("@info", "Your changes will take effect the next time you log in.\n\nGenerating Locales; don't turn off the computer yet.")
        }
        Kirigami.InlineMessage {
            id: installFontMsg
            Layout.fillWidth: true

            type: Kirigami.MessageType.Information

            text: i18nc("@info", "Locales have been generated, but you may want to install fonts yourself")
        }
        Kirigami.InlineMessage {
            id: manualInstallMsg
            Layout.fillWidth: true

            type: Kirigami.MessageType.Error

            text: i18nc("@info", "Failed to generate locales. You should enable it and install font packages")
        }
        Kirigami.InlineMessage {
            id: installSuccessMsg
            Layout.fillWidth: true

            type: Kirigami.MessageType.Positive

            text: i18nc("@info", "Necessary locale and language packages have been installed. It is now safe to turn off the computer.")
        }
        QQC2.Label {
            text: i18n("Language will also be used for formats settings.")
        }
    }

    Connections {
        target: kcm
        function onStartGenerateLocale() {
            takeEffectNextTimeMsg.visible = true;
        }
        function onRequireInstallFont() {
            installFontMsg.visible = true;
        }
        function onAllManual() {
            installFontMsg.visible = true;
        }
        function onGenerateFinished() {
            installSuccessMsg.visible = true;
        }
    }

    view: ListView {
        model: kcm.optionsModel
        delegate: Kirigami.BasicListItem {
            text: model.name
            subtitle: {
                if (model.page === "lang") {
                    return model.localeName;
                }

                if (model.localeName) {
                    return model.example + i18n(" (Formats for %1)", model.localeName);
                } else {
                    return model.example;
                }
            }
            reserveSpaceForSubtitle: true
            onClicked: {
                while (kcm.depth !== 1) {
                    // LocaleSelectPage is cached via Loader
                    if (langPageSelected) {
                        kcm.cacheLangPage(kcm.takeLast());
                    } else {
                        kcm.takeLast();
                    }
                }

                if (model.page === "lang") {
                    if (kcm.cachedLangPage()) {
                        kcm.push(kcm.cachedLangPage());
                    } else {
                        languageSelectPage.active = true;
                        kcm.push(languageSelectPage.item);
                    }
                    langPageSelected = true;
                } else {
                    langPageSelected = false;
                    if (!localeListPage.active) {
                        localeListPage.active = true;
                    }
                    localeListPage.item.setting = model.page;
                    kcm.push(localeListPage.item);
                }
            }
        }
    }

    Loader {
        id: languageSelectPage
        active: false
        source: "AdvancedLanguageSelectPage.qml"
    }

    Loader {
        id: localeListPage
        active: false
        sourceComponent: KCM.ScrollViewKCM {
            property string setting: "lang"
            property alias filterText: searchField.text
            title: {
                localeListView.currentIndex = -1;
                localeListModel.selectedConfig = setting;
                switch (setting) {
                case "numeric":
                    return i18n("Numbers");
                case "time":
                    return i18n("Time");
                case "currency":
                    return i18n("Currency");
                case "measurement":
                    return i18n("Measurements");
                case "collate":
                    return i18n("Collate");
                }
            }

            LocaleListModel {
                id: localeListModel
            }

            header: Kirigami.SearchField {
                id: searchField
                Layout.fillWidth: true
                onTextChanged: localeListModel.filter = text
            }

            view: ListView {
                id: localeListView
                clip: true
                model: localeListModel
                delegate: Kirigami.BasicListItem {
                    icon: model.flag
                    text: model.display
                    subtitle: model.example ? model.example : ''
                    trailing: QQC2.Label {
                        color: Kirigami.Theme.disabledTextColor
                        text: model.localeName
                    }

                    onClicked: {
                        if (model.localeName !== i18n("Default")) {
                            switch (setting) {
                            case "lang":
                                kcm.settings.lang = localeName;
                                break;
                            case "numeric":
                                kcm.settings.numeric = localeName;
                                break;
                            case "time":
                                kcm.settings.time = localeName;
                                break;
                            case "currency":
                                kcm.settings.monetary = localeName;
                                break;
                            case "measurement":
                                kcm.settings.measurement = localeName;
                                break;
                            case "collate":
                                kcm.settings.collate = localeName;
                                break;
                            }
                        } else {
                            kcm.unset(setting);
                        }

                        kcm.takeLast();
                    }
                }
            }
        }
    }
}
