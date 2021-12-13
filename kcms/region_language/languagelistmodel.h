/*
    languagelistmodel.cpp
    SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>
    SPDX-FileCopyrightText: 2019 Kevin Ottens <kevin.ottens@enioka.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include <QAbstractListModel>
class SelectedLanguageModel;
class RegionAndLangSettings;
class LanguageListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(SelectedLanguageModel *selectedLanguageModel READ selectedLanguageModel CONSTANT)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString numberExample READ numberExample NOTIFY exampleChanged)
    Q_PROPERTY(QString currencyExample READ currencyExample NOTIFY exampleChanged)
    Q_PROPERTY(QString timeExample READ timeExample NOTIFY exampleChanged)
    Q_PROPERTY(QString metric READ metric NOTIFY exampleChanged)
    Q_PROPERTY(bool isPreviewExample READ isPreviewExample WRITE setIsPreviewExample NOTIFY isPreviewExampleChanged)
public:
    enum Roles { NativeName = Qt::DisplayRole, LanguageCode, Flag };
    explicit LanguageListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    SelectedLanguageModel *selectedLanguageModel() const;

    int currentIndex() const;
    void setCurrentIndex(int index);
    QString numberExample() const;
    QString currencyExample() const;
    QString timeExample() const;
    QString metric() const;

    // currently unused, but we need it if we want preview examples in add langauge overlay
    bool isPreviewExample() const;
    void setIsPreviewExample(bool preview);

    Q_INVOKABLE QString envLang() const;
    Q_INVOKABLE QString envLanguage() const;
    Q_INVOKABLE void setRegionAndLangSettings(QObject *settings);
Q_SIGNALS:
    void currentIndexChanged();
    void exampleChanged();
    void isPreviewExampleChanged();

protected:
    friend class SelectedLanguageModel;
    static QString languageCodeToName(const QString &languageCode);

private:
    QString exampleHelper(std::function<QString(const QLocale &)> func) const;
    RegionAndLangSettings *m_settings{nullptr};
    QList<QString> m_availableLanguages;
    SelectedLanguageModel *m_selectedLanguageModel;
    int m_index = -1;
    bool m_isPreviewExample = false;
};

class SelectedLanguageModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SelectedLanguageModel(LanguageListModel *parent);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void setRegionAndLangSettings(RegionAndLangSettings *settings);

    Q_INVOKABLE void move(int from, int to);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void addLanguage(const QString &lang);
Q_SIGNALS:
    void exampleChanged();

private:
    void saveLanguages();
    RegionAndLangSettings *m_settings{nullptr};
    QList<QString> m_selectedLanguages;
};
