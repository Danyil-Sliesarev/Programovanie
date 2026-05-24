#pragma once
#include <QSortFilterProxyModel>
#include <QString>

class AccountFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit AccountFilterProxyModel(QObject *parent = nullptr);
    void setTextFilter(const QString &text);
    void setCategoryFilter(const QString &category);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
private:
    QString m_text;
    QString m_category;
};
