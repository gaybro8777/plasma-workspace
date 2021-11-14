/*
    SPDX-FileCopyrightText: 2014 John Layt <john@layt.net>
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2019 Kevin Ottens <kevin.ottens@enioka.com>
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "localegenerator.h"
#include "debug.h"
#include <QFile>
#ifdef OS_UBUNTU
#include <PackageKit/Daemon>
#endif
LocaleGenerator::LocaleGenerator(QObject *parent)
    : QObject(parent)
{
#ifdef GLIBC_LOCALE
    m_interface = new LocaleGenHelper(QStringLiteral("org.kde.localegenhelper"), QStringLiteral("/LocaleGenHelper"), QDBusConnection::systemBus(), this);
    qCDebug(KCM_REGIONANDLANG) << "connect: " << m_interface->isValid();
    connect(m_interface, &LocaleGenHelper::success, this, [this](bool success) {
        if (success) {
#ifdef OS_UBUNTU
            Q_EMIT this->success();
#else
            Q_EMIT this->needsFont();
#endif
        } else {
            Q_EMIT this->allManual();
        }
    });
#endif
}
QString LocaleGenerator::supportMode() const
{
#ifdef OS_UBUNTU
    return QStringLiteral("all");
#elif GLIBC_LOCALE
    return QStringLiteral("glibc");
#else
    return QStringLiteral("none");
#endif
}

/* localesGenerate
 * Why so many macros?
 * We have three cases
 * Ubuntu, can install fonts, translation and generate glibc locale using check-language-support
 * Glibc distro that require glibc locale generating, but we have no way of finding out what fonts need install
 * Such as Debian, Arch...
 *
 * Glibc distro that already generated all the glibc locales
 * Fedora, CentOS... We don't have to do anything, but to remind user that they should install fonts
 *
 * Musl distro that don't even use Glibc locales
 * Nothing we can do, tell them to use musl-locales
 *
 * */
void LocaleGenerator::localesGenerate(const QStringList &list)
{
    qCDebug(KCM_REGIONANDLANG) << "enable locales: " << list;
#ifdef OS_UBUNTU
    ubuntuInstall(list);
    return;
#endif
    if (!QFile(QStringLiteral("/etc/locale.gen")).exists()) {
#ifdef GLIBC_LOCALE
        // fedora or centos
        Q_EMIT success();
#else
        // probably musl distro
        Q_EMIT allManual();
#endif
        return;
    }
#ifdef GLIBC_LOCALE
    else {
        qDebug() << "send polkit request";
        m_interface->enableLocales(list);
    }
#endif
}

#ifdef OS_UBUNTU
void LocaleGenerator::ubuntuInstall(const QStringList &locales)
{
    // Ubuntu
    if (!m_proc) {
        m_proc = new QProcess(this);
    }
    QStringList args;
    args.reserve(locales.size() * 2);
    for (const auto &locale : locales) {
        auto locale_stripped = locale;
        locale_stripped.remove(QStringLiteral(".UTF-8"));
        args.append({QStringLiteral("-l"), locale_stripped});
    }
    m_proc->setProgram("/usr/bin/check-language-support");
    m_proc->setArguments(args);
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &LocaleGenerator::ubuntuLangCheck);
    m_proc->start();
}
void LocaleGenerator::ubuntuLangCheck(int status, QProcess::ExitStatus exit)
{
    Q_UNUSED(exit)
    if (status != 0) {
        // Something wrong with this Ubuntu, don't try futher
        Q_EMIT allManual();
        return;
    }
    const QString output = QString::fromUtf8(m_proc->readAllStandardOutput().simplified());
    const QStringList packages = output.split(QLatin1Char(' '));

    auto transaction = PackageKit::Daemon::resolve(packages, PackageKit::Transaction::FilterNotInstalled | PackageKit::Transaction::FilterArch);
    connect(transaction, &PackageKit::Transaction::package, this, [this](PackageKit::Transaction::Info info, const QString &packageID, const QString &summary) {
        Q_UNUSED(info);
        Q_UNUSED(summary);
        m_packageIDs << packageID;
    });
    connect(transaction, &PackageKit::Transaction::errorCode, this, [](PackageKit::Transaction::Error error, const QString &details) {
        qCDebug(KCM_REGIONANDLANG) << "resolve error" << error << details;
    });
    connect(transaction, &PackageKit::Transaction::finished, this, [packages, this](PackageKit::Transaction::Exit status, uint code) {
        qCDebug(KCM_REGIONANDLANG) << "resolve finished" << status << code << m_packageIDs;
        if (m_packageIDs.size() != packages.size()) {
            qCWarning(KCM_REGIONANDLANG) << "Not all missing packages managed to resolve!" << packages << m_packageIDs;
        }
        auto transaction = PackageKit::Daemon::installPackages(m_packageIDs);
        connect(transaction, &PackageKit::Transaction::errorCode, this, [](PackageKit::Transaction::Error error, const QString &details) {
            qCDebug(KCM_REGIONANDLANG) << "install error:" << error << details;
        });
        connect(transaction, &PackageKit::Transaction::finished, this, [this](PackageKit::Transaction::Exit status, uint code) {
            qCDebug(KCM_REGIONANDLANG) << "install finished:" << status << code;
            Q_EMIT success();
        });
    });
}
#endif
