/*
    optionsmodel.h
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include <QAbstractListModel>
#include <array>

class RegionAndLangSettings;
class KCMRegionAndLang;
class OptionsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles { Name = Qt::DisplayRole, Subtitle, Example, Page };
    OptionsModel(KCMRegionAndLang *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void handleLangChange();

private:
    QString numberExample() const;
    QString timeExample() const;
    QString currencyExample() const;
    QString measurementExample() const;
    QLocale localeWithDefault(const QString &val) const;
    std::array<std::pair<QString, QString>, 5> m_staticNames; // title, page

    RegionAndLangSettings *m_settings;
};
