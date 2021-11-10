/*
  SPDX-FileCopyrightLabel: 2021 Han Young <hanyoung@protonmail.com>

  SPDX-License-Identifier: LGPL-3.0-or-later
*/
import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.15 as Kirigami
import org.kde.kcm 1.2 as KCM
import LocaleListModel 1.0

KCM.ScrollViewKCM {
    property bool langPageSelected: false

    id: root
    implicitHeight: Kirigami.Units.gridUnit * 40
    implicitWidth: Kirigami.Units.gridUnit * 20
    Kirigami.InlineMessage {
        id: takeEffectNextTimeMsg
        text: i18n("Your changes will take effect the next time you log in.")
    }
    Kirigami.InlineMessage {
        id: installFontMsg
        text: i18n("System locales have been generated, but you may want to install fonts for the language")
    }
    Kirigami.InlineMessage {
        id: allManualMsg
        text: i18n("You may want to generate system locales and install the fonts")
    }

    Connections {
        target: kcm
        function onStartGenerateLocale() {
            header = takeEffectNextTimeMsg;
            takeEffectNextTimeMsg.visible = true;
        }
        function onRequireInstallFont() {
            installFontMsg.visible = true;
        }
        function onAllManual() {
            allManualMsg.visible = true;
        }
        function onGenerateFinished() {
            console.log("all set");
        }
    }

    view: ListView {
        model: kcm.optionsModel
        delegate: Kirigami.BasicListItem {
            text: model.name + " - " + model.localeName
            subtitle: model.example
            reserveSpaceForSubtitle: true
            onClicked: {
                if (model.page === "lang") {
                    if (kcm.cachedLangPage()) {
                        kcm.push(kcm.cachedLangPage());
                    } else {
                        languageSelectPage.active = true;
                        kcm.push(languageSelectPage.item);
                        langPageSelected = true;
                    }
                    return;
                }

                if (langPageSelected) {
                    langPageSelected = false;
                    kcm.cacheLangPage(kcm.takeLast());
                }

                if (kcm.depth === 1) {
                    localeListPage.active = true;
                    localeListPage.item.setting = model.page;
                    kcm.push(localeListPage.item);
                } else {
                    kcm.getSubPage(0).setting = model.page;
                    kcm.getSubPage(0).filterText = '';
                    kcm.currentIndex = 1;
                }
            }
        }
    }

    Loader {
        id: languageSelectPage
        active: false
        source: "LanguageSelectPage.qml"
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
                case "lang":
                    return i18n("Region");
                case "numeric":
                    return i18n("Number");
                case "time":
                    return i18n("Time");
                case "currency":
                    return i18n("Currency");
                case "measurement":
                    return i18n("Measurement");
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

                        kcm.currentIndex = 0;
                    }
                }
            }
        }
    }
}
