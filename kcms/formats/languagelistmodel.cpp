/*
    languagelistmodel.h
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>
    SPDX-FileCopyrightText: 2019 Kevin Ottens <kevin.ottens@enioka.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "languagelistmodel.h"
#include "debug.h"
#include "exampleutility.cpp"
#include "formatssettings.h"
#include "kcmformats.h"

#include <unordered_map>

#include <KLocalizedString>
#include <QLocale>
LanguageListModel::LanguageListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_selectedLanguageModel(new SelectedLanguageModel(this))
{
    auto langs = KLocalizedString::availableDomainTranslations("plasmashell").values();
    langs.sort();
    m_availableLanguages = std::move(langs);
}

int LanguageListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_availableLanguages.size();
}
QVariant LanguageListModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    if (row < 0 || row >= m_availableLanguages.size()) {
        return {};
    }
    switch (role) {
    case NativeName:
        return languageCodeToName(m_availableLanguages.at(row));
    case LanguageCode:
        return m_availableLanguages.at(row);
    case Flag: {
        QString flagCode;
        const QStringList split = QLocale(m_availableLanguages.at(row)).name().split(QLatin1Char('_'));
        if (split.size() > 1) {
            flagCode = split[1].toLower();
        }
        QString flagIconPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kf5/locale/countries/%1/flag.png").arg(flagCode));
        return flagIconPath;
    }
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> LanguageListModel::roleNames() const
{
    return {{NativeName, "nativeName"}, {LanguageCode, "languageCode"}, {Flag, "flag"}};
}

QString LanguageListModel::languageCodeToName(const QString &languageCode)
{
    const QLocale locale(languageCode);
    QString languageName = locale.nativeLanguageName();

    if (languageName.isEmpty()) {
        return languageCode;
    }

    if (languageCode.contains(QLatin1Char('@'))) {
        return i18nc("%1 is language name, %2 is language code name", "%1 (%2)", languageName, languageCode);
    }

    //    if (locale.name() != languageCode && m_availableLanguages.contains(locale.name())) {
    //        // KDE languageCode got translated by QLocale to a locale code we also have on
    //        // the list. Currently this only happens with pt that gets translated to pt_BR.
    //        if (languageCode == QLatin1String("pt")) {
    //            return QLocale(QStringLiteral("pt_PT")).nativeLanguageName();
    //        }

    //        return i18nc("%1 is language name, %2 is language code name", "%1 (%2)", languageName, languageCode);
    //    }

    return languageName;
}

int LanguageListModel::currentIndex() const
{
    return m_index;
}

void LanguageListModel::setCurrentIndex(int index)
{
    if (index == m_index || index < 0 || index >= m_availableLanguages.size()) {
        return;
    }

    m_index = index;
    Q_EMIT exampleChanged();
}

QString LanguageListModel::exampleHelper(std::function<QString(const QLocale &)> func) const
{
    if (m_settings) {
        if (m_index < 0) {
            return func(QLocale(m_settings->lang()));
        } else {
            return func(QLocale(m_availableLanguages[m_index]));
        }
    } else {
        return QString();
    }
}

QString LanguageListModel::numberExample() const
{
    return exampleHelper(Utility::numericExample);
}

QString LanguageListModel::currencyExample() const
{
    return exampleHelper(Utility::monetaryExample);
}

QString LanguageListModel::timeExample() const
{
    return exampleHelper(Utility::shortTimeExample);
}

QString LanguageListModel::metric() const
{
    return exampleHelper(Utility::measurementExample);
}

void LanguageListModel::setFormatsSettings(QObject *settings)
{
    if (FormatsSettings *formatsettings = dynamic_cast<FormatsSettings *>(settings)) {
        m_settings = formatsettings;
        m_selectedLanguageModel->setFormatsSettings(formatsettings);
        auto index = std::find_if(m_availableLanguages.begin(), m_availableLanguages.end(), [this](const QString &lang) {
            return m_settings->lang().startsWith(lang);
        });
        if (index != m_availableLanguages.end()) {
            m_index = std::distance(m_availableLanguages.begin(), index);
            Q_EMIT currentIndexChanged();
        }
        Q_EMIT exampleChanged();
    }
}

void SelectedLanguageModel::setFormatsSettings(FormatsSettings *settings)
{
    m_settings = settings;

    beginResetModel();
    if (m_settings->language() == m_settings->defaultLanguageValue() && m_settings->lang() != m_settings->defaultLangValue()) {
        m_selectedLanguages = {m_settings->lang()};
    } else {
        m_selectedLanguages = m_settings->language().split(QLatin1Char(':'));
    }
    endResetModel();
}

SelectedLanguageModel *LanguageListModel::selectedLanguageModel() const
{
    return m_selectedLanguageModel;
}

SelectedLanguageModel::SelectedLanguageModel(LanguageListModel *parent)
    : QAbstractListModel(parent)
{
}

int SelectedLanguageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_selectedLanguages.size();
}
QVariant SelectedLanguageModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)
    auto row = index.row();
    if (row < 0 || row >= m_selectedLanguages.size()) {
        return {};
    }

    return LanguageListModel::languageCodeToName(m_selectedLanguages.at(row));
}

void SelectedLanguageModel::move(int from, int to)
{
    if (from == to || from < 0 || from >= m_selectedLanguages.size() || to < 0 || to >= m_selectedLanguages.size()) {
        return;
    }

    beginResetModel();
    m_selectedLanguages.move(from, to);
    endResetModel();
    saveLanguages();
}

void SelectedLanguageModel::remove(int index)
{
    if (index < 0 || index >= m_selectedLanguages.size()) {
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);
    m_selectedLanguages.removeAt(index);
    endRemoveRows();
}

void SelectedLanguageModel::addLanguages(const QStringList &langs)
{
    if (langs.empty()) {
        return;
    }
    QStringList unique_langs;
    for (const auto &lang : langs) {
        if (m_selectedLanguages.indexOf(lang) == -1) {
            unique_langs.push_back(lang);
        }
    }
    if (!unique_langs.empty()) {
        beginInsertRows(QModelIndex(), m_selectedLanguages.size(), m_selectedLanguages.size() + unique_langs.size() - 1);
        std::copy(unique_langs.begin(), unique_langs.end(), std::back_inserter(m_selectedLanguages));
        endInsertRows();
        saveLanguages();
    }
}

void SelectedLanguageModel::saveLanguages()
{
    if (m_selectedLanguages.empty() || !m_settings) {
        return;
    }

    QString languages;
    for (auto i = m_selectedLanguages.begin(); i != m_selectedLanguages.end(); i++) {
        languages.push_back(*i);
        // no ':' at end
        if (i + 1 != m_selectedLanguages.end()) {
            languages.push_back(QLatin1Char(':'));
        }
    }
    m_settings->setLanguage(languages);
}
