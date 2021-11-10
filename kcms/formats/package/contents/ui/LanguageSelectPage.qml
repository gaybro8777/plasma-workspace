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
            languageListModel.setFormatsSettings(kcm.settings);
        }
    }

    view: ListView {
        property string currentLang: ''
        id: languageView
        model: languageListModel
        currentIndex: languageListModel.currentIndex
        delegate: Kirigami.BasicListItem {
            text: model.nativeName
            icon: model.flag
            trailing: QQC2.Label {
                color: Kirigami.Theme.disabledTextColor
                text: model.languageCode
            }
            onClicked: {
                languageView.currentLang = model.languageCode;
                languageListModel.currentIndex = index;
            }
        }
    }

    footer: ColumnLayout {
        id: footerLayout
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

        RowLayout {
            Layout.alignment: Qt.AlignRight
            QQC2.Button {
                text: i18nc("@action:button", "Advanced Settings")

                onClicked: kcm.push("AdvancedLanguageSelectPage.qml", {"languageListModel": languageListModel})
            }

            QQC2.Button {
                text: i18nc("@action:button", "Apply")
                enabled: !kcm.settings.lang.startsWith(languageView.currentLang);
                onClicked: {
                    kcm.settings.lang = languageView.currentLang;
                    kcm.cacheLangPage(kcm.takeLast());
                }
            }
        }
    }
}
