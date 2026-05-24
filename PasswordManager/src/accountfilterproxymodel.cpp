#include "accountfilterproxymodel.h"
#include "accounttablemodel.h"

AccountFilterProxyModel::AccountFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setDynamicSortFilter(true);
}

void AccountFilterProxyModel::setTextFilter(const QString &text)
{
    m_text = text.trimmed();
    invalidateFilter();
}

void AccountFilterProxyModel::setCategoryFilter(const QString &category)
{
    m_category = category;
    invalidateFilter();
}

bool AccountFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const auto *model = sourceModel();
    if (!model) return true;

    const auto title = model->data(model->index(sourceRow, AccountTableModel::Title, sourceParent)).toString();
    const auto username = model->data(model->index(sourceRow, AccountTableModel::Username, sourceParent)).toString();
    const auto website = model->data(model->index(sourceRow, AccountTableModel::Website, sourceParent)).toString();
    const auto category = model->data(model->index(sourceRow, AccountTableModel::Category, sourceParent)).toString();

    const bool textOk = m_text.isEmpty()
        || title.contains(m_text, Qt::CaseInsensitive)
        || username.contains(m_text, Qt::CaseInsensitive)
        || website.contains(m_text, Qt::CaseInsensitive);
    const bool categoryOk = m_category.isEmpty() || m_category == "All" || category.compare(m_category, Qt::CaseInsensitive) == 0;
    return textOk && categoryOk;
}
