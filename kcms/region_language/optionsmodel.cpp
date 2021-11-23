/*
    optionsmodel.cpp
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include <KLocalizedString>

#include "exampleutility.cpp"
#include "kcmregionandlang.h"
#include "optionsmodel.h"
#include "regionandlangsettings.h"

OptionsModel::OptionsModel(KCMRegionAndLang *parent)
    : QAbstractListModel(parent)
    , m_settings(parent->settings())
{
    m_staticNames = {{{i18n("Language"), QStringLiteral("lang")},
                      {i18n("Numbers"), QStringLiteral("numeric")},
                      {i18n("Time"), QStringLiteral("time")},
                      {i18n("Currency"), QStringLiteral("currency")},
                      {i18n("Measurements"), QStringLiteral("measurement")}}};
    connect(m_settings, &RegionAndLangSettings::langChanged, this, &OptionsModel::handleLangChange);
    connect(m_settings, &RegionAndLangSettings::numericChanged, this, [this] {
        Q_EMIT dataChanged(createIndex(1, 0), createIndex(1, 0), {Subtitle, Example});
    });
    connect(m_settings, &RegionAndLangSettings::timeChanged, this, [this] {
        Q_EMIT dataChanged(createIndex(2, 0), createIndex(2, 0), {Subtitle, Example});
    });
    connect(m_settings, &RegionAndLangSettings::monetaryChanged, this, [this] {
        Q_EMIT dataChanged(createIndex(3, 0), createIndex(3, 0), {Subtitle, Example});
    });
    connect(m_settings, &RegionAndLangSettings::measurementChanged, this, [this] {
        Q_EMIT dataChanged(createIndex(4, 0), createIndex(4, 0), {Subtitle, Example});
    });

    // initialize examples
    QLocale lang = QLocale(m_settings->lang());
    if (m_settings->numeric() == m_settings->defaultNumericValue()) {
        m_numberExample = Utility::numericExample(lang);
    } else {
        m_numberExample = Utility::numericExample(QLocale(m_settings->numeric()));
    }
    if (m_settings->time() == m_settings->defaultTimeValue()) {
        m_timeExample = Utility::timeExample(lang);
    } else {
        m_timeExample = Utility::timeExample(QLocale(m_settings->time()));
    }
    if (m_settings->measurement() == m_settings->defaultMeasurementValue()) {
        m_measurementExample = Utility::measurementExample(lang);
    } else {
        m_measurementExample = Utility::measurementExample(QLocale(m_settings->measurement()));
    }
    if (m_settings->monetary() == m_settings->defaultMonetaryValue()) {
        m_currencyExample = Utility::monetaryExample(lang);
    } else {
        m_currencyExample = Utility::monetaryExample(QLocale(m_settings->monetary()));
    }
}
int OptionsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_staticNames.size();
}
QVariant OptionsModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (row < 0 || row >= (int)m_staticNames.size())
        return QVariant();

    switch (role) {
    case Name:
        return m_staticNames[row].first;
    case Subtitle: {
        switch (row) {
        case 0:
            return m_settings->lang();
        case 1:
            return m_settings->numeric();
        case 2:
            return m_settings->time();
        case 3:
            return m_settings->monetary();
        case 4:
            return m_settings->measurement();
        default:
            return QVariant();
        }
    }
    case Example: {
        switch (row) {
        case 0:
            return QString();
        case 1:
            return m_numberExample;
        case 2:
            return m_timeExample;
        case 3:
            return m_currencyExample;
        case 4:
            return m_measurementExample;
        default:
            return QVariant();
        }
    }
    case Page:
        return m_staticNames[row].second;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> OptionsModel::roleNames() const
{
    return {{Name, "name"}, {Subtitle, "localeName"}, {Example, "example"}, {Page, "page"}};
}

void OptionsModel::handleLangChange()
{
    Q_EMIT dataChanged(createIndex(0, 0), createIndex(0, 0), {Subtitle, Example});
    QLocale lang = QLocale(m_settings->lang());
    if (m_settings->numeric() == m_settings->defaultNumericValue()) {
        m_numberExample = Utility::numericExample(lang);
        Q_EMIT dataChanged(createIndex(1, 0), createIndex(1, 0), {Subtitle, Example});
    }
    if (m_settings->time() == m_settings->defaultTimeValue()) {
        m_timeExample = Utility::timeExample(lang);
        Q_EMIT dataChanged(createIndex(2, 0), createIndex(2, 0), {Subtitle, Example});
    }
    if (m_settings->measurement() == m_settings->defaultMeasurementValue()) {
        m_measurementExample = Utility::measurementExample(lang);
        Q_EMIT dataChanged(createIndex(3, 0), createIndex(3, 0), {Subtitle, Example});
    }
    if (m_settings->monetary() == m_settings->defaultMonetaryValue()) {
        m_currencyExample = Utility::monetaryExample(lang);
        Q_EMIT dataChanged(createIndex(4, 0), createIndex(4, 0), {Subtitle, Example});
    }
}
