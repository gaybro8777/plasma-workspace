/*
 *  localelistmodel.cpp
 *  Copyright 2014 Sebastian Kügler <sebas@kde.org>
 *  Copyright 2021 Han Young <hanyoung@protonmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */
#include "localelistmodel.h"
#include "exampleutility.cpp"
#include "kcmregionandlang.h"
#include <KLocalizedString>
#include <QTextCodec>
LocaleListModel::LocaleListModel()
{
    QList<QLocale> m_locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    m_localeTuples.reserve(m_locales.size() + 1);
    m_localeTuples.push_back(std::tuple<QString, QString, QString, QString, QString, QLocale>(i18n("Default for "),
                                                                                              QString(),
                                                                                              i18n("System"),
                                                                                              QString(),
                                                                                              i18n("Default"),
                                                                                              QLocale()));
    for (auto &locale : m_locales) {
        m_localeTuples.push_back(std::tuple<QString, QString, QString, QString, QString, QLocale>(locale.nativeLanguageName(),
                                                                                                  QLocale::languageToString(locale.language()),
                                                                                                  locale.nativeCountryName(),
                                                                                                  QLocale::countryToString(locale.country()),
                                                                                                  locale.name(),
                                                                                                  locale));
    }
}
int LocaleListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (m_noFilter) {
        return m_localeTuples.size();
    } else {
        return m_filteredLocales.size();
    }
}
QVariant LocaleListModel::data(const QModelIndex &index, int role) const
{
    int tupleIndex;
    if (m_noFilter) {
        tupleIndex = index.row();
    } else {
        tupleIndex = m_filteredLocales.at(index.row());
    }

    const auto &[lang, engLang, country, engCountry, name, locale] = m_localeTuples.at(tupleIndex);
    switch (role) {
    case FlagIcon: {
        QString flagCode;
        const QStringList split = name.split(QLatin1Char('_'));
        if (split.size() > 1) {
            flagCode = split[1].toLower();
        }
        auto flagIconPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kf5/locale/countries/%1/flag.png").arg(flagCode));
        return flagIconPath;
    }
    case DisplayName: {
        // 0 is unset option, 1 is locale C
        if (tupleIndex == 1) {
            return name;
        }
        const QString clabel = !country.isEmpty() ? country : engCountry;
        QString languageName;
        if (!lang.isEmpty()) {
            languageName = lang;
        } else {
            languageName = name;
        }
        if (tupleIndex == 0) {
            return languageName + QStringLiteral(" ") + clabel;
        } else {
            return languageName + QStringLiteral(" (") + clabel + QStringLiteral(")");
        }
    }
    case LocaleName: {
        return name;
    }
    case Example: {
        switch (m_configType) {
        case Lang:
            return QVariant();
        case Numeric:
            return Utility::numericExample(locale);
        case Time:
            return Utility::timeExample(locale);
        case Currency:
            return Utility::monetaryExample(locale);
        case Measurement:
            return Utility::measurementExample(locale);
        case Collate:
            return Utility::collateExample(locale);
        default:
            return QVariant();
        }
    }
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> LocaleListModel::roleNames() const
{
    return {{LocaleName, "localeName"}, {DisplayName, "display"}, {FlagIcon, "flag"}, {Example, "example"}};
}

const QString &LocaleListModel::filter() const
{
    return m_filter;
}

void LocaleListModel::setFilter(const QString &filter)
{
    if (m_filter == filter) {
        return;
    }
    m_filter = filter;
    filterLocale();
}

void LocaleListModel::filterLocale()
{
    beginResetModel();
    if (!m_filter.isEmpty()) {
        m_filteredLocales.clear();
        int i{0};
        for (const auto &[lang, engLang, country, engCountry, name, _locale] : m_localeTuples) {
            if (lang.indexOf(m_filter, 0, Qt::CaseInsensitive) != -1) {
                m_filteredLocales.push_back(i);
            } else if (country.indexOf(m_filter, 0, Qt::CaseInsensitive) != -1) {
                m_filteredLocales.push_back(i);
            } else if (name.indexOf(m_filter, 0, Qt::CaseInsensitive) != -1) {
                m_filteredLocales.push_back(i);
            } else if (engCountry.indexOf(m_filter, 0, Qt::CaseInsensitive) != -1) {
                m_filteredLocales.push_back(i);
            } else if (engLang.indexOf(m_filter, 0, Qt::CaseInsensitive) != -1) {
                m_filteredLocales.push_back(i);
            }
            i++;
        }
        m_noFilter = false;
    } else {
        m_noFilter = true;
    }
    endResetModel();
}

QString LocaleListModel::selectedConfig() const
{
    switch (m_configType) {
    case Lang:
        return QStringLiteral("lang");
    case Numeric:
        return QStringLiteral("numeric");
    case Time:
        return QStringLiteral("time");
    case Currency:
        return QStringLiteral("currency");
    case Measurement:
        return QStringLiteral("measurement");
    case Collate:
        return QStringLiteral("collate");
    }
    // won't reach here
    return QString();
}

void LocaleListModel::setSelectedConfig(const QString &config)
{
    if (config == QStringLiteral("lang")) {
        m_configType = Lang;
    } else if (config == QStringLiteral("numeric")) {
        m_configType = Numeric;
    } else if (config == QStringLiteral("time")) {
        m_configType = Time;
    } else if (config == QStringLiteral("measurement")) {
        m_configType = Measurement;
    } else if (config == QStringLiteral("currency")) {
        m_configType = Currency;
    } else {
        m_configType = Collate;
    }
    Q_EMIT selectedConfigChanged();
    Q_EMIT dataChanged(createIndex(0, 0), createIndex(rowCount(), 0), QVector<int>(1, Example));
}

void LocaleListModel::setLang(const QString &lang)
{
    QString tmpLang;
    bool isC = false;
    if (lang.isEmpty()) {
        tmpLang = qgetenv("LANG");
        if (tmpLang.isEmpty()) {
            tmpLang = QStringLiteral("C");
            isC = true;
        }
    } else {
        tmpLang = lang;
    }

    auto &[languageName, engLang, country, engCountry, name, locale] = m_localeTuples.front();
    if (isC) {
        languageName = i18n("System Default");
        country = tmpLang;
        engCountry = tmpLang;
    } else {
        languageName = i18nc("Inherit value from setting $X, default value for this field", "Default for");
        country = QLocale(tmpLang).nativeLanguageName();
        engCountry = QLocale::countryToString(QLocale(tmpLang).country());
    }
    locale = QLocale(tmpLang);

    Q_EMIT dataChanged(createIndex(0, 0), createIndex(0, 0));
}
