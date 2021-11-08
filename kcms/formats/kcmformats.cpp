/*
    kcmformats.cpp
    SPDX-FileCopyrightText: 2014 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QCollator>

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>

#include "formatssettings.h"
#include "kcmformats.h"
#include "languagelistmodel.h"
#include "localelistmodel.h"
#include "optionsmodel.h"
#include "localegenerator.h"

K_PLUGIN_CLASS_WITH_JSON(KCMFormats, "kcm_formats.json")

KCMFormats::KCMFormats(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : KQuickAddons::ManagedConfigModule(parent, data, args)
{
    KAboutData *aboutData = new KAboutData(QStringLiteral("kcm_formats"),
                                           i18nc("@title", "Formats"),
                                           QStringLiteral("0.1"),
                                           QLatin1String(""),
                                           KAboutLicense::LicenseKey::GPL_V2,
                                           i18nc("@info:credit", "Copyright 2021 Han Young"));

    aboutData->addAuthor(i18nc("@info:credit", "Han Young"), i18nc("@info:credit", "Author"), QStringLiteral("hanyoung@protonmail.com"));

    setAboutData(aboutData);
    setQuickHelp(i18n("You can configure the formats used for time, dates, money and other numbers here."));

    qmlRegisterAnonymousType<FormatsSettings>("kcmformats", 1);
    qmlRegisterAnonymousType<OptionsModel>("kcmformats_optionsmodel", 1);
    qmlRegisterAnonymousType<SelectedLanguageModel>("kcmformats_selectedLanguageModel", 1);
    qmlRegisterType<LocaleListModel>("LocaleListModel", 1, 0, "LocaleListModel");
    qmlRegisterType<LanguageListModel>("LanguageListModel", 1, 0, "LanguageListModel");
    qmlRegisterType<LocaleGenerator>("LocaleGenerator", 1, 0, "LocaleGenerator");
    m_settings = new FormatsSettings(this);
    m_optionsModel = new OptionsModel(this);
}

void KCMFormats::save()
{
    // shouldn't have data race issue
    if (!m_generator) {
        m_generator = new LocaleGenerator(this);
        connect(m_generator, &LocaleGenerator::allManual, this, &KCMFormats::allManual);
        connect(m_generator, &LocaleGenerator::success, this, &KCMFormats::generateFinished);
        connect(m_generator, &LocaleGenerator::needsFont, this, &KCMFormats::requireInstallFont);
    }

    // assemble full locales in use
    QStringList locales;
    if (settings()->lang() != settings()->defaultLangValue()) {
        locales.append(settings()->lang());
    }
    if (settings()->numeric() != settings()->defaultNumericValue()) {
        locales.append(settings()->numeric());
    }
    if (settings()->time() != settings()->defaultTimeValue()) {
        locales.append(settings()->time());
    }
    if (settings()->measurement() != settings()->defaultMeasurementValue()) {
        locales.append(settings()->measurement());
    }
    if (settings()->monetary() != settings()->defaultMonetaryValue()) {
        locales.append(settings()->monetary());
    }

    if (!locales.isEmpty()) {
        m_generator->localesGenerate(locales);
        Q_EMIT startGenerateLocale();
    }
    ManagedConfigModule::save();
    Q_EMIT takeEffectNextTime();
}

FormatsSettings *KCMFormats::settings() const
{
    return m_settings;
}

OptionsModel *KCMFormats::optionsModel() const
{
    return m_optionsModel;
}
QQuickItem *KCMFormats::getSubPage(int index) const
{
    return subPage(index);
}

void KCMFormats::unset(const QString &setting)
{
    const char *entry;
    if (setting == QStringLiteral("lang")) {
        entry = "LANG";
        settings()->setLang(settings()->defaultLangValue());
    } else if (setting == QStringLiteral("numeric")) {
        entry = "LC_NUMERIC";
        settings()->setNumeric(settings()->defaultNumericValue());
    } else if (setting == QStringLiteral("time")) {
        entry = "LC_TIME";
        settings()->setTime(settings()->defaultTimeValue());
    } else if (setting == QStringLiteral("measurement")) {
        entry = "LC_MEASUREMENT";
        settings()->setMeasurement(settings()->defaultMeasurementValue());
    } else {
        entry = "LC_MONETARY";
        settings()->setMonetary(settings()->defaultMonetaryValue());
    }
    settings()->config()->group(QStringLiteral("Formats")).deleteEntry(entry);
}
#include "kcmformats.moc"
