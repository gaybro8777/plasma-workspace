/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "historymodel.h"
#include "historyimageitem.h"
#include "historystringitem.h"
#include "historyurlitem.h"

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_maxSize(0)
    , m_displayImages(true)
{
}

HistoryModel::~HistoryModel()
{
    removeRows(0, m_items.count(), QModelIndex(), true);
}

void HistoryModel::clear()
{
    removeRows(0, m_items.count());
}

void HistoryModel::setMaxSize(int size)
{
    if (m_maxSize == size) {
        return;
    }
    QMutexLocker lock(&m_mutex);
    m_maxSize = size;
    int excess = unpinnedCount() - m_maxSize;
    if (excess > 0) {
        removeRows(m_items.count() - excess, excess, QModelIndex());
    }
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_items.count();
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.count() || index.column() != 0) {
        return QVariant();
    }

    QSharedPointer<HistoryItem> item = m_items.at(index.row());
    HistoryItemType type = HistoryItemType::Text;
    if (dynamic_cast<HistoryStringItem *>(item.data())) {
        type = HistoryItemType::Text;
    } else if (dynamic_cast<HistoryImageItem *>(item.data())) {
        type = HistoryItemType::Image;
    } else if (dynamic_cast<HistoryURLItem *>(item.data())) {
        type = HistoryItemType::Url;
    }

    switch (role) {
    case Qt::DisplayRole:
        return item->text();
    case Qt::DecorationRole:
        return item->image();
    case Qt::UserRole:
        return QVariant::fromValue<HistoryItemConstPtr>(qSharedPointerConstCast<const HistoryItem>(item));
    case Qt::UserRole + 1:
        return item->uuid();
    case Qt::UserRole + 2:
        return QVariant::fromValue<HistoryItemType>(type);
    case Qt::UserRole + 3:
        return item->uuid().toBase64();
    case Qt::UserRole + 4:
        return int(type);
    case Qt::UserRole + 5:
        return item->pinned();
    }
    return QVariant();
}

bool HistoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return removeRows(row, count, QModelIndex(), false);
}

bool HistoryModel::removeRows(int row, int count, const QModelIndex &parent, bool removeAll)
{
    if (parent.isValid()) {
        return false;
    }
    if ((row + count) > m_items.count()) {
        return false;
    }
    QMutexLocker lock(&m_mutex);
    int removed = 0;
    int total = count;
    while (count) {
        auto index = row + count - 1;
        if (removeAll || !m_items.at(index)->pinned()) {
            beginRemoveRows(QModelIndex(), index, index);
            m_items.removeAt(index);
            endRemoveRows();
            ++removed;
        }
        --count;
    }
    return (removed == total);
}

bool HistoryModel::remove(const QByteArray &uuid)
{
    QModelIndex index = indexOf(uuid);
    if (!index.isValid()) {
        return false;
    }
    return removeRows(index.row(), 1, QModelIndex(), true);
}


bool HistoryModel::pinned(const QByteArray &uuid)
{
    QModelIndex index = indexOf(uuid);
    if (!index.isValid()) {
        return false;
    }

    QSharedPointer<HistoryItem> item = m_items.at(index.row());
    return item->pinned();
}

QList<int> HistoryModel::pinnedRows() const
{
    QList<int> rows;
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i)->pinned()) {
            rows << i;
        }
    }
    return rows;
}

void HistoryModel::pin(const QByteArray &uuid)
{
    QModelIndex index = indexOf(uuid);
    if (!index.isValid()) {
        return;
    }

    QSharedPointer<HistoryItem> item = m_items.at(index.row());
    item->pin();
    emit dataChanged(index, index);

    // respect maxSize
    if (!item->pinned()) {
        int excess = unpinnedCount() - m_maxSize;
        if (excess > 0) {
            removeRow(m_items.count() - 1, QModelIndex());
        }
    }

    if (index.row() > 0) {
       sortItem(item);
    }
}

// moving the item to the pinned/unpinned section
// from top to bottom:
// [current clipboard section]
// [pinned section]
// [unpinned section]
void HistoryModel::sortItem(QSharedPointer<HistoryItem> item) {
    QMutexLocker lock(&m_mutex);
    QModelIndex index = indexOf(item->uuid());
    int toRow = -1;
    if (item->pinned()) {
        // The top item is either pinned or unpinned
        // Pinning an item which is different than the top one
        // never moves it to the top
        toRow = 1;
        for (int i = 1; i < index.row(); ++i) {
            if (m_items.at(i)->pinned()) {
                ++toRow;
            } else {
                break;
            }
        }
    } else {
        toRow = index.row();
        for (int i = index.row() + 1; i < m_items.count(); ++i) {
            if (m_items.at(i)->pinned()) {
                toRow = i + 1;
            } else {
                break;
            }
        }
    }
    if (toRow > 0 && toRow != index.row() && toRow != index.row() + 1) {
        beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), toRow);
        m_items.move(index.row(), toRow - (item->pinned() ? 0 : 1));
        endMoveRows();
    }
}

int HistoryModel::unpinnedCount() {
    int unpinnedCount = 0;
    for (int i = 0; i < m_items.count(); ++i) {
        if (!m_items.at(i)->pinned()) {
            ++unpinnedCount;
        }
    }
    return unpinnedCount;
}

QModelIndex HistoryModel::indexOf(const QByteArray &uuid) const
{
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i)->uuid() == uuid) {
            return index(i);
        }
    }
    return QModelIndex();
}

QModelIndex HistoryModel::indexOf(const HistoryItem *item) const
{
    if (!item) {
        return QModelIndex();
    }
    return indexOf(item->uuid());
}

void HistoryModel::insert(QSharedPointer<HistoryItem> item)
{
    if (item.isNull()) {
        return;
    }
    const QModelIndex existingItem = indexOf(item.data());
    if (existingItem.isValid()) {
        // move to top
        moveToTop(existingItem.row());
        return;
    }

    QMutexLocker lock(&m_mutex);

    if (unpinnedCount() == m_maxSize) {
        // remove last item
        if (m_maxSize == 0) {
            // special case - cannot insert any items
            return;
        }
        beginRemoveRows(QModelIndex(), m_items.count() - 1, m_items.count() - 1);
        m_items.removeLast();
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), 0, 0);
    item->setModel(this);
    m_items.prepend(item);
    endInsertRows();

    // move the previous top to the right pinned/unpinned section
    if (m_items.count() > 1 && !m_items.at(1)->pinned()) {
        sortItem(m_items.at(1));
    }
}

void HistoryModel::moveToTop(const QByteArray &uuid)
{
    const QModelIndex existingItem = indexOf(uuid);
    if (!existingItem.isValid()) {
        return;
    }
    moveToTop(existingItem.row());
}

void HistoryModel::moveToTop(int row)
{
    if (row == 0 || row >= m_items.count()) {
        return;
    }
    QMutexLocker lock(&m_mutex);
    beginMoveRows(QModelIndex(), row, row, QModelIndex(), 0);
    m_items.move(row, 0);
    endMoveRows();

    // move the 2nd to the right pinned/unpinned section
    if (m_items.count() > 1 && !m_items.at(1)->pinned()) {
        sortItem(m_items.at(1));
    }
}

void HistoryModel::moveTopToBack()
{
    if (m_items.count() < 2) {
        return;
    }
    QMutexLocker lock(&m_mutex);
    beginMoveRows(QModelIndex(), 0, 0, QModelIndex(), m_items.count());
    auto item = m_items.takeFirst();
    m_items.append(item);
    endMoveRows();
}

void HistoryModel::moveBackToTop()
{
    moveToTop(m_items.count() - 1);
}

QHash<int, QByteArray> HistoryModel::roleNames() const
{
    QHash<int, QByteArray> hash;
    hash.insert(Qt::DisplayRole, QByteArrayLiteral("DisplayRole"));
    hash.insert(Qt::DecorationRole, QByteArrayLiteral("DecorationRole"));
    hash.insert(Qt::UserRole + 3, QByteArrayLiteral("UuidRole"));
    hash.insert(Qt::UserRole + 4, QByteArrayLiteral("TypeRole"));
    hash.insert(Qt::UserRole + 5, QByteArrayLiteral("PinnedRole"));
    return hash;
}
