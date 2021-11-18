/*
    kcmformats.cpp
    SPDX-FileCopyrightText: 2014 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QCollator>
#include <QQuickItem>

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

K_PLUGIN_CLASS_WITH_JSON(KCMFormats, "kcm_regionandlang.json")

KCMFormats::KCMFormats(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : KQuickAddons::ManagedConfigModule(parent, data, args)
{
    KAboutData *aboutData = new KAboutData(QStringLiteral("kcm_regionandlang"),
                                           i18nc("@title", "Region & Language"),
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

void KCMFormats::cacheLangPage(QQuickItem *langPage)
{
    m_langPage = langPage;
}
QQuickItem *KCMFormats::cachedLangPage()
{
    return m_langPage;
}
QString KCMFormats::toGlibcLocale(const QString &lang) const
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
    return KDELocaleToGlibcLocale[lang];
}
#include "kcmformats.moc"
