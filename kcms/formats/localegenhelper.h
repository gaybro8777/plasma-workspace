/*
    localegenhelper.h
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include <PolkitQt1/Authority>
#include <QCoreApplication>
#include <QObject>
#include <QProcess>
#include <QDBusContext>
class QTimer;
class LocaleGenHelper : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.localegenhelper.LocaleGenHelper")
public:
    LocaleGenHelper();
    Q_SCRIPTABLE void enableLocales(const QStringList &locales, qint64 pid);
Q_SIGNALS:
    Q_SCRIPTABLE void success(bool success);
private Q_SLOTS:
    void enableLocalesPrivate(PolkitQt1::Authority::Result result);
    void handleLocaleGen(int statusCode, QProcess::ExitStatus status);

private:
    bool editLocaleGen();
    void exitAfterTimeOut();
    PolkitQt1::Authority *m_authority;
    QStringList m_locales;
    QTimer *m_timer;
};
