/*
    languagelistmodel.h
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>
    SPDX-FileCopyrightText: 2019 Kevin Ottens <kevin.ottens@enioka.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "languagelistmodel.h"
#include "debug.h"
#include "exampleutility.cpp"
#include "kcmregionandlang.h"
#include "regionandlangsettings.h"

#include <unordered_map>

#include <KLocalizedString>
#include <QLocale>
LanguageListModel::LanguageListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_selectedLanguageModel(new SelectedLanguageModel(this))
{
    connect(this, &LanguageListModel::isPreviewExampleChanged, this, &LanguageListModel::exampleChanged);
    connect(m_selectedLanguageModel, &SelectedLanguageModel::exampleChanged, this, &LanguageListModel::exampleChanged);
    auto langs = KLocalizedString::availableDomainTranslations("plasmashell").values();
    langs.sort();
    langs.push_front(QStringLiteral("C"));
    m_availableLanguages = std::move(langs);
    m_index = -1;
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
    static auto getLangWithDefault = [this] {
        QString defaultLang = m_settings->defaultLangValue();
        if (m_settings->lang() != defaultLang) {
            return QLocale(m_settings->lang());
        } else {
            return QLocale::system();
        }
    };
    if (m_settings) {
        if (m_isPreviewExample) {
            if (m_index < 0) {
                return func(getLangWithDefault());
            } else {
                return func(QLocale(m_availableLanguages[m_index]));
            }
        } else {
            return func(getLangWithDefault());
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
    return exampleHelper(Utility::timeExample);
}

QString LanguageListModel::metric() const
{
    return exampleHelper(Utility::measurementExample);
}

void LanguageListModel::setRegionAndLangSettings(QObject *settings)
{
    if (RegionAndLangSettings *regionandlangsettings = dynamic_cast<RegionAndLangSettings *>(settings)) {
        m_settings = regionandlangsettings;
        m_selectedLanguageModel->setRegionAndLangSettings(regionandlangsettings);
        Q_EMIT exampleChanged();
    }
}
bool LanguageListModel::isPreviewExample() const
{
    return m_isPreviewExample;
}
void LanguageListModel::setIsPreviewExample(bool preview)
{
    m_isPreviewExample = preview;
}

void SelectedLanguageModel::setRegionAndLangSettings(RegionAndLangSettings *settings)
{
    m_settings = settings;

    beginResetModel();
    if (m_settings->language().isEmpty() && m_settings->lang() != m_settings->defaultLangValue()) {
        // no language but have lang
        m_selectedLanguages = {m_settings->lang()};
    } else if (!m_settings->language().isEmpty()) {
        // have language, ignore lang
        m_selectedLanguages = m_settings->language().split(QLatin1Char(':'));
    } else {
        // have nothing, figure out from env
        QString lang = envLang();
        QString language = envLanguage();
        if (!language.isEmpty()) {
            m_selectedLanguages = language.split(QLatin1Char(':'));
        } else if (!lang.isEmpty()) {
            m_selectedLanguages = {lang};
        }
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
    if (row < 0 || row > m_selectedLanguages.size()) {
        return QVariant();
    }
    // "add Language" Item
    if (row == m_selectedLanguages.size()) {
        return QVariant();
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
    Q_EMIT exampleChanged();
}

void SelectedLanguageModel::remove(int index)
{
    if (index < 0 || index >= m_selectedLanguages.size()) {
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);
    m_selectedLanguages.removeAt(index);
    endRemoveRows();
    saveLanguages();
    Q_EMIT exampleChanged();
}

void SelectedLanguageModel::addLanguage(const QString &lang)
{
    if (lang.isEmpty() || m_selectedLanguages.indexOf(lang) != -1) {
        return;
    }

    beginInsertRows(QModelIndex(), m_selectedLanguages.size(), m_selectedLanguages.size());
    m_selectedLanguages.push_back(lang);
    endInsertRows();
    saveLanguages();
    Q_EMIT exampleChanged();
}

void SelectedLanguageModel::saveLanguages()
{
    if (!m_settings) {
        return;
    }
    if (m_selectedLanguages.empty()) {
        m_settings->setLang(m_settings->defaultLangValue());
        m_settings->config()->group(QStringLiteral("Formats")).deleteEntry("lang");
        m_settings->config()->group(QStringLiteral("Translations")).deleteEntry("language");
    } else {
        m_settings->setLang(KCMRegionAndLang::toGlibcLocale(m_selectedLanguages.front()));
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
}
QString SelectedLanguageModel::envLang() const
{
    return qgetenv("LANG");
}
QString SelectedLanguageModel::envLanguage() const
{
    return qgetenv("LANGUAGE");
}
