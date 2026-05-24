#pragma once
#include "accountentry.h"
#include "accountrepository.h"
#include <QAbstractTableModel>
#include <QList>
#include <memory>

class AccountTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column { Id = 0, Title, Username, Password, Website, Category, UpdatedAt, LeakStatus, Notes, ColumnCount };
    explicit AccountTableModel(QObject *parent = nullptr);

    void setRepository(AccountRepository *repository);
    void setItems(const QList<AccountEntry> &items);
    QList<AccountEntry> items() const;
    AccountEntry itemAt(int row) const;
    int idAt(int row) const;
    void updateLeakStatusById(int id, const QString &status);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

private:
    QList<AccountEntry> m_items;
    AccountRepository *m_repository = nullptr;
};
