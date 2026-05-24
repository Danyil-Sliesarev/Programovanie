#include "accounttablemodel.h"
#include <QBrush>
#include <QColor>

AccountTableModel::AccountTableModel(QObject *parent) : QAbstractTableModel(parent) {}

void AccountTableModel::setRepository(AccountRepository *repository) { m_repository = repository; }

void AccountTableModel::setItems(const QList<AccountEntry> &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}

QList<AccountEntry> AccountTableModel::items() const { return m_items; }

AccountEntry AccountTableModel::itemAt(int row) const
{
    if (row < 0 || row >= m_items.size()) return {};
    return m_items.at(row);
}

int AccountTableModel::idAt(int row) const
{
    if (row < 0 || row >= m_items.size()) return 0;
    return m_items.at(row).id;
}

void AccountTableModel::updateLeakStatusById(int id, const QString &status)
{
    for (int row = 0; row < m_items.size(); ++row) {
        if (m_items[row].id == id) {
            m_items[row].leakStatus = status;
            const QModelIndex idx = index(row, LeakStatus);
            emit dataChanged(idx, idx, {Qt::DisplayRole, Qt::EditRole});
            return;
        }
    }
}

int AccountTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

int AccountTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant AccountTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) return {};
    const AccountEntry &e = m_items[index.row()];

    if (role == Qt::BackgroundRole && index.column() == LeakStatus) {
        if (e.leakStatus.contains("Pwned", Qt::CaseInsensitive)) return QBrush(QColor(255, 220, 220));
        if (e.leakStatus.contains("Safe", Qt::CaseInsensitive)) return QBrush(QColor(220, 255, 220));
        if (e.leakStatus.contains("Error", Qt::CaseInsensitive)) return QBrush(QColor(255, 245, 200));
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) return {};

    switch (index.column()) {
    case Id: return e.id;
    case Title: return e.title;
    case Username: return e.username;
    case Password:
        return role == Qt::DisplayRole ? QString("••••••••") : e.password;
    case Website: return e.website;
    case Category: return e.category;
    case UpdatedAt: return e.updatedAt;
    case LeakStatus: return e.leakStatus;
    case Notes: return e.notes;
    default: return {};
    }
}

QVariant AccountTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section) {
    case Id: return "ID";
    case Title: return "Name";
    case Username: return "Username";
    case Password: return "Password";
    case Website: return "URL";
    case Category: return "Category";
    case UpdatedAt: return "Updated";
    case LeakStatus: return "Leak Check";
    case Notes: return "Notes";
    default: return {};
    }
}

Qt::ItemFlags AccountTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() != Id && index.column() != UpdatedAt && index.column() != LeakStatus) {
        f |= Qt::ItemIsEditable;
    }
    return f;
}

bool AccountTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole || index.row() < 0 || index.row() >= m_items.size()) return false;
    AccountEntry updated = m_items[index.row()];
    const QString text = value.toString();

    switch (index.column()) {
    case Title: updated.title = text; break;
    case Username: updated.username = text; break;
    case Password: updated.password = text; updated.leakStatus = "Not checked"; break;
    case Website: updated.website = text; break;
    case Category: updated.category = text; break;
    case Notes: updated.notes = text; break;
    default: return false;
    }

    if (m_repository && !m_repository->update(updated)) return false;
    if (m_repository) {
        const auto reloaded = m_repository->loadAll();
        for (const auto &item : reloaded) {
            if (item.id == updated.id) {
                updated.updatedAt = item.updatedAt;
                updated.leakStatus = item.leakStatus;
                break;
            }
        }
    }
    m_items[index.row()] = updated;
    emit dataChanged(this->index(index.row(), 0), this->index(index.row(), ColumnCount - 1), {Qt::DisplayRole, Qt::EditRole});
    return true;
}
