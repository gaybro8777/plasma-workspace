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
    static std::unordered_map<QString, QString> KDELocaleToGlibcLocale = {
        {QStringLiteral("sv"), QStringLiteral("sv_SE.UTF-8")},           {QStringLiteral("sk"), QStringLiteral("sk_SK.UTF-8")},
        {QStringLiteral("hu"), QStringLiteral("hu_HU.UTF-8")},           {QStringLiteral("sr@ijekavianlatin"), QStringLiteral("sr_RS@latin.UTF-8")},
        {QStringLiteral("sr@ijekavian"), QStringLiteral("sr_ME.UTF-8")}, {QStringLiteral("az"), QStringLiteral("az_AZ.UTF-8")},
        {QStringLiteral("ml"), QStringLiteral("ml_IN.UTF-8")},           {QStringLiteral("pt"), QStringLiteral("pt_PT.UTF-8")},
        {QStringLiteral("ar"), QStringLiteral("ar_AE.UTF-8")},           {QStringLiteral("es"), QStringLiteral("es_ES.UTF-8")},
        {QStringLiteral("da"), QStringLiteral("da_DK.UTF-8")},           {QStringLiteral("pa"), QStringLiteral("pa_IN.UTF-8")},
        {QStringLiteral("bs"), QStringLiteral("bs_BA.UTF-8")},           {QStringLiteral("et"), QStringLiteral("et.EE.UTF-8")},
        {QStringLiteral("lt"), QStringLiteral("lt_LT.UTF-8")},           {QStringLiteral("sr@latin"), QStringLiteral("sr_RS@latin.UTF-8")},
        {QStringLiteral("cs"), QStringLiteral("cs_CZ.UTF-8")},           {QStringLiteral("ro"), QStringLiteral("ro_RO.UTF-8")},
        {QStringLiteral("fi"), QStringLiteral("fi_FI.UTF-8")},           {QStringLiteral("eu"), QStringLiteral("eu_ES.UTF-8")},
        {QStringLiteral("tr"), QStringLiteral("tr_TR.UTF-8")},           {QStringLiteral("en_US"), QStringLiteral("en_US.UTF-8")},
        {QStringLiteral("pl"), QStringLiteral("pl_PL.UTF-8")},           {QStringLiteral("uk"), QStringLiteral("uk_UA.UTF-8")},
        {QStringLiteral("ja"), QStringLiteral("ja_JP.UTF-8")},           {QStringLiteral("sl"), QStringLiteral("sl_SI.UTF-8")},
        {QStringLiteral("nds"), QStringLiteral("nds_DE.UTF-8")},         {QStringLiteral("ca@valencia"), QStringLiteral("ca_ES@valencia.UTF-8")},
        {QStringLiteral("pt_BR"), QStringLiteral("pt_BR.UTF-8")},        {QStringLiteral("vi"), QStringLiteral("vi_VN.UTF-8")},
        {QStringLiteral("hi"), QStringLiteral("hi_IN.UTF-8")},           {QStringLiteral("nl"), QStringLiteral("nl_NL.UTF-8")},
        {QStringLiteral("ru"), QStringLiteral("ru_RU.UTF-8")},           {QStringLiteral("ca"), QStringLiteral("ca_ES.UTF-8")},
        {QStringLiteral("fr"), QStringLiteral("fr_FR.UTF-8")},           {QStringLiteral("nn"), QStringLiteral("nn_NO.UTF-8")},
        {QStringLiteral("nb"), QStringLiteral("nb_NO.UTF-8")},           {QStringLiteral("he"), QStringLiteral("he_IL.UTF-8")},
        {QStringLiteral("ia"), QStringLiteral("ia_FR.UTF-8")},           {QStringLiteral("ta"), QStringLiteral("ta_IN.UTF-8")},
        {QStringLiteral("en_GB"), QStringLiteral("en_GB.UTF-8")},        {QStringLiteral("ko"), QStringLiteral("ko_KR.UTF-8")},
        {QStringLiteral("de"), QStringLiteral("de_DE.UTF-8")},           {QStringLiteral("it"), QStringLiteral("it_IT.UTF-8")},
        {QStringLiteral("gl"), QStringLiteral("gl_ES.UTF-8")},           {QStringLiteral("zh_TW"), QStringLiteral("zh_TW.UTF-8")},
        {QStringLiteral("id"), QStringLiteral("id_ID.UTF-8")},           {QStringLiteral("lv"), QStringLiteral("lv_LV.UTF-8")},
        {QStringLiteral("sr"), QStringLiteral("sr_RS.UTF-8")},           {QStringLiteral("tg"), QStringLiteral("tg_TJ.UTF-8")},
        {QStringLiteral("el"), QStringLiteral("el_GR.UTF-8")},           {QStringLiteral("zh_CN"), QStringLiteral("zh_CN.UTF-8")}};
    auto langs = KLocalizedString::availableDomainTranslations("plasmashell").values();
    std::for_each(langs.begin(), langs.end(), [](QString &KDECode) {
        if (KDELocaleToGlibcLocale.count(KDECode)) {
            KDECode = KDELocaleToGlibcLocale[KDECode];
        } else {
            qCWarning(KCM_FORMATS) << "Can't convert KDE language code " << KDECode << " to glibc locale code";
        }
    });
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
