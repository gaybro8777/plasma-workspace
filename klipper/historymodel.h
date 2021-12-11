/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QAbstractListModel>
#include <QRecursiveMutex>

class HistoryItem;

enum class HistoryItemType {
    Text,
    Image,
    Url,
};

class HistoryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit HistoryModel(QObject *parent = nullptr);
    ~HistoryModel() override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent, bool removeAll);
    QHash<int, QByteArray> roleNames() const override;
    bool remove(const QByteArray &uuid);

    bool pinned(const QByteArray &uuid);
    void pin(const QByteArray &uuid);
    QList<int> pinnedRows() const;

    int maxSize() const;
    void setMaxSize(int size);

    bool displayImages() const;
    void setDisplayImages(bool show);

    void clear();
    void moveToTop(const QByteArray &uuid);
    void moveTopToBack();
    void moveBackToTop();

    QModelIndex indexOf(const QByteArray &uuid) const;
    QModelIndex indexOf(const HistoryItem *item) const;

    void insert(QSharedPointer<HistoryItem> item);

    QRecursiveMutex *mutex()
    {
        return &m_mutex;
    }

private:
    void moveToTop(int row);
    void sortItem(QSharedPointer<HistoryItem> item);
    int unpinnedCount();
    QList<int> m_pinnedItems;
    QList<QSharedPointer<HistoryItem>> m_items;
    int m_maxSize;
    bool m_displayImages;
    QRecursiveMutex m_mutex;
};

inline int HistoryModel::maxSize() const
{
    return m_maxSize;
}

inline bool HistoryModel::displayImages() const
{
    return m_displayImages;
}

inline void HistoryModel::setDisplayImages(bool show)
{
    m_displayImages = show;
}

Q_DECLARE_METATYPE(HistoryItemType)
