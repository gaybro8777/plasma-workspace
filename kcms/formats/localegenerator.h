/*
    SPDX-FileCopyrightText: 2014 John Layt <john@layt.net>
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2019 Kevin Ottens <kevin.ottens@enioka.com>
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once
#include "localegenhelperinterface.h"
#include <QObject>
#include <QProcess>
using LocaleGenHelper = org::kde::localegenhelper::LocaleGenHelper;
class LocaleGenerator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString supportMode READ supportMode CONSTANT)
public:
    LocaleGenerator(QObject *parent = nullptr);

    QString supportMode() const;

    Q_INVOKABLE void localesGenerate(const QStringList &list);
#ifdef OS_UBUNTU
    void ubuntuInstall(const QString &lang);
#endif
Q_SIGNALS:
    void success();
    void needsFont();
    void allManual();
#ifdef OS_UBUNTU
private Q_SLOTS:
    void ubuntuLangCheck(int statusCode, QProcess::ExitStatus status);
#endif
private:
    QProcess *m_proc;
    LocaleGenHelper *m_interface;
    QStringList m_packageIDs;
};