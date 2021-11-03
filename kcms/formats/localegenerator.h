/*
    SPDX-FileCopyrightText: 2014 John Layt <john@layt.net>
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2019 Kevin Ottens <kevin.ottens@enioka.com>
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once
#include <QObject>
#include <QProcess>
class LocaleGenerator : public QObject
{
    Q_OBJECT
public:
    LocaleGenerator(QObject *parent);
    void localesGenerate(const QStringList &list);

    void ubuntuInstall(const QString &lang);

Q_SIGNALS:
    void success();
    void needsFont();
    void allManual();

private Q_SLOTS:
    void ubuntuLangCheck(int statusCode, QProcess::ExitStatus status);
private:
    QProcess *m_proc;

    QStringList m_packageIDs;
};

