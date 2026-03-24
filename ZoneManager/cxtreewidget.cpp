#include "cxtreewidget.h"
#include <QHeaderView>
#include "QCString.h"
#include <algorithm>

CXTreeWidget::CXTreeWidget(QWidget *parent): QTreeWidget(parent)
{
    header()->setMinimumSectionSize(500);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    setDropIndicatorShown(true);
}

CXTreeWidget::~CXTreeWidget()
{

}

QTreeWidgetItem *CXTreeWidget::NewLineItem(char name[])
{
    QTreeWidgetItem *FldItem = nullptr;
    QTreeWidgetItem *item = nullptr;
    char fold[MAX_ky_name];
    char nm[MAX_ky_name];
    bool HasFold = GetFolder_Name(name, fold, nm);

    if (strlen(fold) > 0)
    {
        FldItem = GetFolderParent(fold);
        if (FldItem == nullptr)
        {
            FldItem = NewFolderItem(fold);
        }
        item = new QTreeWidgetItem(QStringList() << tr2(nm), 1);
        item->setData(0, Qt::UserRole + 1, tr2(name));
        item->setCheckState(0, Qt::Unchecked);
        item->setIcon(0, QIcon(":/Routeplugin/resources/File.png"));
        FldItem->addChild(item);
    }
    else
    {
        item = new QTreeWidgetItem(QStringList() << tr2(nm), 1);
        item->setData(0, Qt::UserRole + 1, tr2(nm));
        item->setCheckState(0, Qt::Unchecked);
        item->setIcon(0, QIcon(":/Routeplugin/resources/File.png"));
        this->addTopLevelItem(item);

    }

    return item;
}

QTreeWidgetItem *CXTreeWidget::NewFolderItem(char name[])
{
    QTreeWidgetItem *item = GetFolderParent(name);
    if (item != nullptr)
    {
        return item;
    }
    else
    {
        item = new QTreeWidgetItem(QStringList() << tr2(name));
        item->setIcon(0, QIcon(":/Routeplugin/resources/folder.png"));
        item->setCheckState(0, Qt::Unchecked);
        item->setData(0, Qt::UserRole + 1, tr2(name));
        this->addTopLevelItem(item);
        return item;
    }
}

QTreeWidgetItem *CXTreeWidget::GetFolderParent(char name[])
{
    QTreeWidgetItem *item = NULL;
    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        if ((*it)->type() == 1)
        {
            ++it;
            continue;
        }
        if ((*it)->text(0) == tr2(name))
        {
            item = *it;
            return item;
        }
        ++it;
    }
    return item;
}

bool CXTreeWidget::GetFolder_Name(const char all[], char fold[], char name[])
{
    fold[0] = name[0] = 0;
    int i = 0;
	int nCount = (std::min)((int)strlen(all), MAX_ky_name);
	for (i = 0; i < nCount; i++)
    {
        if (all[i] == '/')
        {
            break;
        }
    }
	if (i > 0 && i < nCount)
    {
        strncpy(fold, all, i);
        fold[i] = 0;
        strcpy(name, &all[i + 1]);
        return true;
    }
    else
    {
        fold[0] = 0;
        strcpy(name, all);
        return false;
    }
}

void CXTreeWidget::InitTree()
{

}

void CXTreeWidget::setChildCheckState(QTreeWidgetItem *item, Qt::CheckState cs)
{
    if (!item)
    {
        return;
    }

    for (int i = 0; i < item->childCount(); i++)
    {
        QTreeWidgetItem *child = item->child(i);
        if (child->checkState(0) != cs)
        {
            child->setCheckState(0, cs);
        }
    }
    setParentCheckState(item->parent());
}

void CXTreeWidget::setParentCheckState(QTreeWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    int selectedCount = 0;
    int childCount = item->childCount();
    for (int i = 0; i < childCount; i++)
    {
        QTreeWidgetItem *child = item->child(i);
        if (child->checkState(0) == Qt::Checked)
        {
            selectedCount++;
        }
    }

    if (selectedCount == 0)
    {
        item->setCheckState(0, Qt::Unchecked);
    }
    else if (selectedCount == childCount)
    {
        item->setCheckState(0, Qt::Checked);
    }
    else
    {
        item->setCheckState(0, Qt::PartiallyChecked);
    }
}

bool CXTreeWidget::isTopItem(QTreeWidgetItem *item)
{
    if (!item)
    {
        return false;
    }
    if (!item->parent())
    {
        return true;
    }
    return false;
}

bool CXTreeWidget::IsRember(const QString &name)
{
    for (int i = 0; i < m_RemberArray.size(); i++)
    {
        if (name == m_RemberArray[i])
        {
            return true;
        }
    }
    return false;
}

void CXTreeWidget::setItemsCheck()
{
    foreach (QTreeWidgetItem *item, m_listItemCheck)
    {
        if (item->type() == 1 || (item->type() == 0 && item->childCount() == 0))
        {
            item->setCheckState(0, Qt::Checked);
        }
    }
}

