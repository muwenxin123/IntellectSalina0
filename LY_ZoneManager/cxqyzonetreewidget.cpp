#include "cxqyzonetreewidget.h"

#include "LyMessageBox.h"
#include <QDragMoveEvent>
#include <QMouseEvent>
#include <QMenu>
#include "zonemanagewidget.h"
#include "newfoldernamedlg.h"

#include "QCString.h"

CXQYZoneTreeWidget::CXQYZoneTreeWidget(QWidget *parent): CXTreeWidget(parent)
{
    m_CurItem = nullptr;
    m_CurInd = -1;

    itemPopMenu = nullptr;
    itemFolderPopMenu = nullptr;
    treePopMenu = nullptr;

    m_pZoneManageWidget = nullptr;
    m_hItemDragS = nullptr;

    initActions();

}

CXQYZoneTreeWidget::~CXQYZoneTreeWidget()
{
    delete itemPopMenu;
    delete itemFolderPopMenu;
    delete treePopMenu;
}

void CXQYZoneTreeWidget::setupConnection()
{
    connect(this, &QTreeWidget::customContextMenuRequested, this, &CXQYZoneTreeWidget::OnCustomContextMenuRequested, Qt::UniqueConnection);
    connect(this, &QTreeWidget::itemSelectionChanged, this, &CXQYZoneTreeWidget::OnTreeSelectionSlot, Qt::UniqueConnection);
    connect(this, &QTreeWidget::itemChanged, this, &CXQYZoneTreeWidget::OnItemChangedSlot, Qt::UniqueConnection);
}

void CXQYZoneTreeWidget::initActions()
{
    QAction *newFolderAction = new QAction(QIcon(""), tr2("新建文件夹"), this);
    connect(newFolderAction, &QAction::triggered, this, &CXQYZoneTreeWidget::NewFolderSlot);

    QAction *deleteAction = new QAction(QIcon(""), tr2("删除"), this);
    connect(deleteAction, &QAction::triggered, this, &CXQYZoneTreeWidget::OnDeleteSlot);

    QAction *showItemAction = new QAction(QIcon(""), tr2("显示诸元"), this);
    showItemAction->setObjectName("1");
    connect(showItemAction, &QAction::triggered, this, &CXQYZoneTreeWidget::displayPara);

    QAction *showNameAction = new QAction(QIcon(""), tr2("显示名称"), this);
    showNameAction->setObjectName("2");
    connect(showNameAction, &QAction::triggered, this, &CXQYZoneTreeWidget::displayPara);

    QAction *reNameAction = new QAction(QIcon(""), tr2("重命名"), this);
    connect(reNameAction, &QAction::triggered, this, &CXQYZoneTreeWidget::ReNamed);

    QAction *saveAsAction = new QAction(QIcon(""), tr2("另存为"), this);
    connect(saveAsAction, &QAction::triggered, this, &CXQYZoneTreeWidget::SaveAs);

    itemPopMenu = new QMenu(this);
    itemPopMenu->addAction(newFolderAction);
    itemPopMenu->addAction(deleteAction);
    itemPopMenu->addAction(showItemAction);
    itemPopMenu->addAction(showNameAction);

    itemPopMenu->addAction(reNameAction);
    itemPopMenu->addAction(saveAsAction);

    treePopMenu = new QMenu(this);
    treePopMenu->addAction(newFolderAction);

    itemFolderPopMenu = new QMenu(this);

    itemFolderPopMenu->addAction(deleteAction);
    itemFolderPopMenu->addAction(showItemAction);
    itemFolderPopMenu->addAction(showNameAction);

    itemFolderPopMenu->addAction(reNameAction);
}

void CXQYZoneTreeWidget::InitTree()
{
    this->clear();
    m_ZoneArray.clear();
    m_listItemCheck.clear();

    m_dbx.ReadDatabase(&m_ZoneArray);

    QTreeWidgetItem *htItem;
    for (int i = 0; i < m_ZoneArray.size(); i++)
    {
        if (m_ZoneArray[i].data.n == 0)
        {
            htItem = NewFolderItem(m_ZoneArray[i].name);
        }
        else
        {
            htItem = NewLineItem(m_ZoneArray[i].name);
        }

        if (IsRember(tr2(m_ZoneArray[i].name)))
        {
            m_ZoneArray[i].show[0] = 1;
            m_listItemCheck.append(htItem);
        }
    }
    clearSelection();
    scrollToTop();
    setupConnection();
    m_CurItem = nullptr;
    m_CurInd = -1;
    collapseAll();
}

void CXQYZoneTreeWidget::InsertAllTree()
{

}

bool CXQYZoneTreeWidget::DeletOneItem(QTreeWidgetItem *item)
{
    int ind = GetAirLineArrID_ByItem(item);
    if (ind < 0 || ind >= m_ZoneArray.size())
    {
        return false;
    }

    QString strname = item->data(0, Qt::UserRole + 1).toString();

    if (m_dbx.DeleteZoneDatabase(tr2(m_ZoneArray[ind].name)) == true)
    {
        m_ZoneArray.removeAt(ind);
        QTreeWidgetItem *parent = item->parent();
        if (parent)
        {
            parent->removeChild(item);
        }
        else
        {
            delete item;
        }
    }
    else
    {
        return false;
    }
    m_strSelFolderName = "";
    return true;
}

int CXQYZoneTreeWidget::GetAirLineArrID_ByItem(QTreeWidgetItem *item)
{
    int ind = -1;
    QString name;
    QTreeWidgetItem *pParItem = item->parent();
    if (pParItem != NULL)
    {
        name = pParItem->text(0) + "/" + item->text(0);
    }
    else
    {
        name = item->text(0);
    }

    for (int i = 0; i < m_ZoneArray.size(); i++)
    {
        if (name == tr2(m_ZoneArray[i].name))
        {
            return i;
        }
    }
    return ind;
}

bool CXQYZoneTreeWidget::DeleteCurItem()
{
    QString str;
    if (m_CurItem->type())
    {
        str = tr2("你确定要删除区域 %1 ?").arg(m_CurItem->text(0));

        if (LyMessageBox::question(nullptr, tr2("删除询问"), str, QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            return false;
        }
    }
    else
    {
        str = tr2("你确定要删除文件夹 %1 及它内部的所有区域?").arg(m_CurItem->text(0));

        if (LyMessageBox::question(nullptr, tr2("删除询问"), str, QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            return false;
        }

        for (int i = m_CurItem->childCount() - 1; i >= 0; --i)
        {
            DeletOneItem(m_CurItem->child(i));
        }

    }
    if (DeletOneItem(m_CurItem) == true)
    {

        m_CurItem = nullptr;
        m_CurInd = -1;
    }

    return true;
}

void CXQYZoneTreeWidget::OnCustomContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = NULL;
    item = itemAt(pos);

    if (item)
    {
        m_CurItem = item;
        m_CurInd = GetAirLineArrID_ByItem(m_CurItem);
        if (item->type() == 0)
        {
            itemFolderPopMenu->exec(QCursor::pos());
        }
        else if (item->type() == 1)
        {
            itemPopMenu->exec(QCursor::pos());
        }
    }
    else
    {
        treePopMenu->exec(QCursor::pos());
    }
}

void CXQYZoneTreeWidget::OnItemChangedSlot(QTreeWidgetItem *item, int col)
{
    bool flag = false;

    if (Qt::PartiallyChecked != item->checkState(0))
    {
        setChildCheckState(item, item->checkState(0));
        setCurrentItem(item);
        flag = item->checkState(0);
    }

    if (Qt::PartiallyChecked == item->checkState(0))
    {
        if (!isTopItem(item))
        {
            item->parent()->setCheckState(0, Qt::PartiallyChecked);
        }
        setCurrentItem(item);
        flag = item->checkState(0);
    }
    m_CurItem = currentItem();
    m_CurInd = GetAirLineArrID_ByItem(m_CurItem);

    if (m_CurInd != -1)
    {
        m_ZoneArray[m_CurInd].show[0] = flag;
        if (m_pZoneManageWidget)
        {
            if (m_ZoneArray[m_CurInd].show[0] && !m_pZoneManageWidget->m_bCreateFlag && m_ZoneArray[m_CurInd].data.n != 0)
            {
                m_pZoneManageWidget->DingWeiqy(m_ZoneArray[m_CurInd]);
            }
        }

    }
}

void CXQYZoneTreeWidget::OnTreeSelectionSlot()
{
    m_strSelFolderName = "";
    m_CurItem = currentItem();

    if (m_CurItem == nullptr)
    {
        return;
    }
    m_CurInd = GetAirLineArrID_ByItem(m_CurItem);
    if (m_CurInd == -1)
    {
        return;
    }

    if (m_CurItem->type() == 0)
    {
        m_strSelFolderName = m_CurItem->text(0);
    }
    else
    {
        QTreeWidgetItem *parent = m_CurItem->parent();
        if (parent)
        {
            m_strSelFolderName = parent->text(0);
        }
    }

    if (m_pZoneManageWidget != nullptr)
    {
        m_pZoneManageWidget->WriteKyList(m_ZoneArray[m_CurInd]);
    }

}

void CXQYZoneTreeWidget::displayPara()
{
    if (m_CurInd == -1)
    {
        return;
    }
    int flag = -1;
    QString strName = sender()->objectName();
    flag = strName.toInt();

    int oldID = m_CurInd;
    if (m_CurItem->type() == 0)
    {
        for (int i = 0; i < m_CurItem->childCount(); i++)
        {
            m_CurInd = GetAirLineArrID_ByItem(m_CurItem->child(i));
            DispAirLinePara(flag);
        }
        m_CurInd = oldID;
    }
    else
    {
        DispAirLinePara(flag);
    }
}

void CXQYZoneTreeWidget::OnDeleteSlot()
{
    if (m_CurItem)
    {
        DeleteCurItem();
    }
}

void CXQYZoneTreeWidget::DispAirLinePara(int flag)
{
    if (m_CurInd >= 0 && m_CurInd < m_ZoneArray.size() && flag >= 1 && flag <= 4)
    {
        m_ZoneArray[m_CurInd].show[flag] = !(m_ZoneArray[m_CurInd].show[flag]);
    }
}

void CXQYZoneTreeWidget::NewFolderSlot()
{
    NewFolderNameDlg dlg(tr2("新建文件夹"), tr2("新建文件夹"), 20, this, true);
    QString strName;

    if (dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    if (dlg.m_FileName.isEmpty())
    {
        strName = tr2("新建文件夹");
    }
    else
    {
        strName = dlg.m_FileName;
    }
    if (strName.length() >= 20)
    {
        LyMessageBox::warning(nullptr, tr2("警告"), tr2("文件夹名称过长!"));
        return;
    }

    if (IsRepeat(strName) == false)
    {
        QUYU_Zone zone;
        memset(&zone, 0, sizeof(QUYU_Zone));
        strcpy(zone.name, strName.toLocal8Bit().data());
        if (m_dbx.InsertNewZoneTab(zone) == false)
        {
            LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败或文件夹重名!!!"));
            return;
        }
        m_ZoneArray.append(zone);
        m_CurItem = NewFolderItem(zone.name);
        m_CurInd = GetAirLineArrID_ByItem(m_CurItem);
    }
    else
    {
        LyMessageBox::warning(nullptr, tr2("警告"), tr2("文件夹重名,请重新输入!!!"));
        NewFolderSlot();
        return;
    }
}

void CXQYZoneTreeWidget::ReNamed()
{
    QTreeWidgetItem *item = m_CurItem;
    int oldID = m_CurInd;

    if (m_CurItem->type() == 0)
    {
        QString folderName = m_CurItem->text(0);
        NewFolderNameDlg dlg(tr2("文件夹名称"), folderName, 100, this, true);
        if (dlg.exec() != QDialog::Accepted)
        {
            return;
        }
        if (dlg.m_FileName == "" || dlg.m_FileName == folderName)
        {
            return;
        }
        if (dlg.m_FileName.length() >= 20)
        {
            LyMessageBox::warning(this, tr2("警告"), tr2("文件夹名称过长!"));
            return;
        }
        if (IsRepeat(dlg.m_FileName))
        {
            LyMessageBox::warning(this, tr2("警告"), tr2("文件夹重名,请重新输入!!!"));
            ReNamed();
            return;
        }

        m_dbx.DeleteZoneDatabase(tr2(m_ZoneArray[m_CurInd].name));
        strcpy(m_ZoneArray[m_CurInd].name, dlg.m_FileName.toLocal8Bit().data());
        m_dbx.InsertNewZoneTab(m_ZoneArray[m_CurInd]);

        int childcount = m_CurItem->childCount();
        for (int i = 0; i < childcount; i++)
        {
            QTreeWidgetItem *hChildItem = m_CurItem->child(i);
            if (hChildItem == nullptr)
            {
                continue;
            }
            QString name;
            name = hChildItem->text(0);
            if (name.isEmpty())
            {
                return;
            }
            name = dlg.m_FileName + "/" + name;
            m_CurInd = GetAirLineArrID_ByItem(hChildItem);
            m_dbx.DeleteZoneDatabase(tr2(m_ZoneArray[m_CurInd].name));
            strcpy(m_ZoneArray[m_CurInd].name, name.toLocal8Bit().data());
            m_dbx.InsertNewZoneTab(m_ZoneArray[m_CurInd]);
        }
        m_CurInd = oldID;
        m_CurItem->setText(0, dlg.m_FileName);
    }
    else
    {
        if (SaveAs())
        {
            DeletOneItem(item);
        }
    }

}

bool CXQYZoneTreeWidget::SaveAs()
{
    bool ret = false;
    if (m_CurInd < 0 || m_CurInd >= m_ZoneArray.size())
    {
        return ret;
    }

    char nm[MAX_ky_name];
    char fold[MAX_ky_name];
    GetFolder_Name(m_ZoneArray[m_CurInd].name, fold, nm);
    NewFolderNameDlg dlg(tr2("区域名称"), tr2(nm), 100, this, true);
    if (dlg.exec() != QDialog::Accepted)
    {
        return ret;
    }
    if (dlg.m_FileName == "" || dlg.m_FileName == tr2(nm))
    {
        return ret;
    }
    QString name = dlg.m_FileName;
    if (strlen(fold) > 0)
    {
        name = tr2("%1/%2").arg(tr2(fold)).arg(dlg.m_FileName);
    }
    if (IsRepeat(name) == true)
    {
        LyMessageBox::warning(this, tr2("警告"), tr2("区域名称重复，请输入新名称！"));
        SaveAs();
        return ret;
    }

    QUYU_Zone zone = m_ZoneArray[m_CurInd];
    strcpy(zone.name, name.toLocal8Bit().data());
    if (m_dbx.InsertNewZoneTab(zone))
    {
        m_CurItem = NewLineItem(zone.name);
        m_ZoneArray.append(zone);
        m_CurInd = m_ZoneArray.size() - 1;
        m_CurItem->setSelected(true);
        m_pZoneManageWidget->WriteKyList(zone);
        ret = true;
    }
    return ret;
}

bool CXQYZoneTreeWidget::IsRepeat(const QString &str, int *pInd )
{
    for (int i = 0; i < m_ZoneArray.size(); i++)
    {
        if (str == tr2(m_ZoneArray[i].name))
        {
            if (pInd != NULL)
            {
                *pInd = i;
            }
            return true;
        }
    }
    return false;
}

void CXQYZoneTreeWidget::startDrag(Qt::DropActions supportedActions)
{
    QTreeWidgetItem *item = currentItem();
    if (!item)
    {
        return;
    }

    if (item->type() == 0)
    {
        return;
    }
    m_hItemDragS = item;
    QTreeWidget::startDrag(supportedActions);
}

void CXQYZoneTreeWidget::dropEvent(QDropEvent *event)
{
    QTreeWidgetItem *item = m_hItemDragS;
    QTreeWidgetItem *pitem = itemAt(event->pos());

    if (pitem == item || (pitem != nullptr && pitem->type() == 1 && item->type() == 1 && item->parent() == nullptr))
    {
        event->ignore();
        return;
    }
    QString name = item->text(0);
    if (pitem != nullptr)
    {
        QString folderName = pitem->text(0);
        name = folderName + "/" + item->text(0);
    }

    if (pitem != nullptr && pitem->type() == 1 && pitem->parent() == nullptr)
    {
        name = item->text(0);
    }

    if (IsRepeat(name))
    {
        LyMessageBox::warning(nullptr, tr2("警告"), tr2("已有相同文件"));
        return;
    }
    pitem->setExpanded(true);

    int ind = GetAirLineArrID_ByItem(item);
    if (ind == -1)
    {
        return;
    }
    m_dbx.DeleteZoneDatabase(tr2(m_ZoneArray[ind].name));
    strcpy(m_ZoneArray[ind].name, name.toLocal8Bit().data());
    m_dbx.InsertNewZoneTab(m_ZoneArray[ind]);

    m_hItemDragS = nullptr;
    RemberSetcheck();
    InitTree();
    setItemsCheck();
}

void CXQYZoneTreeWidget::RemberSetcheck()
{
    m_RemberArray.clear();
    QString str;
    for (int i = 0; i < m_ZoneArray.size(); i++)
    {
        if (m_ZoneArray[i].show[0] == 1)
        {
            str = tr2(m_ZoneArray[i].name);
            m_RemberArray.append(str);
        }
    }
}

