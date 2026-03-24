#include "IUAVTargetTree.h"
#include "ui_IUAVTargetTree.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDropEvent>

#include "QCString.h"
#include "IUAVNavigationArmShotAlone54Activator.h"
#include "LyMessageBox.h"

#ifdef WIN32
#include <windows.h>
#endif

IUAVTargetTree::IUAVTargetTree(QWidget *parent) :
    LyDialog(tr2("TargetBlead"), parent),
    ui(new Ui::IUAVTargetTree)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1700, 800);
   // Init();
}

IUAVTargetTree::~IUAVTargetTree()
{
    delete ui;
}
void IUAVTargetTree::Init()
{
    connect(ui->pushButton, &QPushButton::clicked, this, &IUAVTargetTree::on_pushButton_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &IUAVTargetTree::on_pushButton_2_clicked);
    connect(ui->treeWidget_2, &QTreeWidget::currentItemChanged, this, &IUAVTargetTree::on_treeWidget_2_currentItemChanged);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &IUAVTargetTree::on_pushButton_3_clicked);
	connect(this, &IUAVTargetTree::MessageBoxWidgetShow, this, &IUAVTargetTree::on_MessageBoxWidgetShow);

    m_TreeMenuRight = new QMenu(this);
    QAction *TaskMenu_Edit = m_TreeMenuRight->addAction(tr("delete"));
    connect(TaskMenu_Edit, SIGNAL(triggered()), this, SLOT(on_Menu_Targetblead_Delete()));
    m_pTreeWidgetVec.push_back(ui->treeWidget);
    m_pTreeWidgetVec.push_back(ui->treeWidget_3);
    m_pTreeWidgetVec.push_back(ui->treeWidget_4);

    QStringList widgetList;
    widgetList << tr2("目标ID") << tr2("置信度(%)") << tr2("经度") << tr2("纬度") << tr2("海拔高度") << tr2("尺寸");
    for (auto &treewidget : m_pTreeWidgetVec)
    {
        treewidget->setColumnCount(widgetList.size());
        treewidget->setHeaderLabels(widgetList);
    }

    QStringList bleadList;
    bleadList << tr2("批号") << tr2("状态") << tr2("关联列表") << tr2("类型") << tr2("属性") << tr2("经度") << tr2("纬度") << tr2("高度") << tr2("速度") << tr2("距离") << tr2("时间");
    ui->treeWidget_2->setColumnCount(bleadList.size());
    ui->treeWidget_2->setHeaderLabels(bleadList);

    connect(ui->treeWidget_2, &QTreeWidget::itemChanged, [&](QTreeWidgetItem * item, int column)
    {
        if (item->checkState(column) == Qt::Checked)
        {
            for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i)
            {
                QTreeWidgetItem *otherwidget = ui->treeWidget_2->topLevelItem(i);
                if (otherwidget != item)
                {
                    otherwidget->setCheckState(column, Qt::Unchecked);
                }
            }
        }
    });

    m_Timer = new QTimer();
    m_Timer->setInterval(10000);
    connect(m_Timer, &QTimer::timeout, this, [&]()
    {
        if (m_AchieveTarget_P.size() <= 0)
        {
            for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); i++)
            {
                QTreeWidgetItem *item = ui->treeWidget_2->topLevelItem(i);
                if (!item)
                {
                    continue;
                }
                for (int j = 0; j < item->columnCount(); j++)
                {
                    item->setForeground(j, QBrush(QColor(255, 0, 0)));
                }
            }
        }
        else
        {
            XygsZC_Mark_01 xygsZC_P = m_AchieveTarget_P[0];

            if (m_pUsedTreeWidgetMap.find(xygsZC_P.componentID) != m_pUsedTreeWidgetMap.end())
            {
                int treeValue = m_pUsedTreeWidgetMap[xygsZC_P.componentID];
                for (int i = 0; i < m_pTreeWidgetVec[treeValue]->topLevelItemCount(); i++)
                {
                    QTreeWidgetItem *item = m_pTreeWidgetVec[treeValue]->topLevelItem(i);
                    if (!item)
                    {
                        continue;
                    }
                    QString parentItemText = item->text(0);

                    for (int j = 0; j < item->childCount(); j++)
                    {
                        QTreeWidgetItem *childerTree = item->child(j);
                        if (childerTree->text(0) != QString::number(xygsZC_P.targetID))
                        {
                            continue;
                        }
                        childerTree->setText(2, QString::number(xygsZC_P.target_lon, 'f', 6));
                        childerTree->setText(3, QString::number(xygsZC_P.target_lat, 'f', 6));
                        childerTree->setText(4, QString::number(xygsZC_P.target_alt));
                        break;
                    }
                    break;
                }
            }

            for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); i++)
            {
                QTreeWidgetItem *item = ui->treeWidget_2->topLevelItem(i);
                if (!item)
                {
                    continue;
                }

                QStringList list = item->text(2).split(";");
                bool bExist = false;

                QString strcomponent = QString::number(xygsZC_P.componentID) + ":" + QString::number(xygsZC_P.targetID);
                for (auto info : list)
                {
                    if (info.contains(strcomponent))
                    {
                        item->setText(1, tr2("通信"));
                        item->setText(5, QString::number(xygsZC_P.target_lon, 'f', 6));
                        item->setText(6, QString::number(xygsZC_P.target_lat, 'f', 6));
                        item->setText(7, QString::number(xygsZC_P.target_alt));
                        item->setText(9, QString::number(xygsZC_P.laserDistance));
                        item->setText(10, QString::number(xygsZC_P.targetTime));
                        for (int j = 0; j <  item->columnCount(); j++)
                        {
                            item->setForeground(j, QBrush(QColor(0, 255, 0)));
                        }
                        bExist = true;
                        break;
                    }
                }
                if (!bExist)
                {
                    item->setForeground(i, QBrush(QColor(255, 0, 0)));
                }
            }
        }
        m_AchieveTarget_P.clear();
        ui->treeWidget_2->clearSelection();
    });
    connect(this, &IUAVTargetTree::SignalsClearTree, this, &IUAVTargetTree::ClearTargetList);

    m_strUAVTarkIDList = IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->GetDiscernUAVIDList();
}
void IUAVTargetTree::SetData(std::map<int, QVector<XygsZC_Mark_01>> xygsZC_Taregt, int nChangeVal)
{
    for (auto info : xygsZC_Taregt)
    {
        if (info.first != nChangeVal)
        {
            continue;
        }

        if (GetUnUsedTreeWidget(info.first) == -1)
        {
            return;
        }
        int mainCommand = GetUnUsedTreeWidget(info.first);

        QTreeWidget *CommandtreeWidget = m_pTreeWidgetVec[mainCommand];

        if (m_OldxygsZC_TaregtMap.find(info.first) == m_OldxygsZC_TaregtMap.end())
        {
            QTreeWidgetItem *parentItem = new QTreeWidgetItem(CommandtreeWidget, QStringList() << QString::number(info.first));
            for (int i = 0; i < info.second.size(); i++)
            {
                QString str = QString::number(info.second[i].targetID);
                int believe = 50;
                if (info.second[i].targetType == 0)
                {
                    believe = 80;
                }
                QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem, QStringList() << str << QString::number(believe)
                                                                 << "-" << "-" << "-" << QString::number(info.second[i].targetRectWidth) + ";" + QString::number(info.second[i].targetRectHeight));

                if (!mainCommand)
                {
                    for (int j = 0; j < childItem->columnCount(); j++)
                    {
                        childItem->setForeground(j, QBrush(QColor(0, 255, 0)));
                    }
                    continue;
                }

                childItem->setFlags(childItem->flags() | Qt::ItemIsUserCheckable);
                childItem->setCheckState(0, Qt::Unchecked);
                for (int j = 0; j < childItem->columnCount(); j++)
                {
                    childItem->setForeground(j, QBrush(QColor(255, 0,  0)));
                }

            }
            CommandtreeWidget->addTopLevelItem(parentItem);
            CommandtreeWidget->expandAll();
        }
        else
        {
            for (int i = 0; i < CommandtreeWidget->topLevelItemCount(); i++)
            {
                QTreeWidgetItem *parentItem = CommandtreeWidget->topLevelItem(i);
                if (parentItem->text(0) != QString::number(info.first))
                {
                    continue;
                }
                QStringList childrenList;
                for (int j = 0; j < parentItem->childCount(); ++j)
                {
                    childrenList << parentItem->child(j)->text(0);
                }

                for (auto targte : info.second)
                {
                    if (childrenList.contains(QString::number(targte.targetID)))
                    {
                        continue;
                    }
                    int believe = 50;
                    if (info.second[i].targetType == 0)
                    {
                        believe = 80;
                    }
                    QTreeWidgetItem *childtree = new QTreeWidgetItem(parentItem, QStringList() << QString::number(targte.targetID) << QString::number(believe)
                                                                     << "-" << "-" << "-" << "-" << QString::number(info.second[i].targetRectWidth) + ";" + QString::number(info.second[i].targetRectHeight));
                    if (!mainCommand)
                    {
                        for (int j = 0; j < CommandtreeWidget->columnCount(); j++)
                        {
                            childtree->setForeground(j, QBrush(QColor(0, 255, 0)));
                        }
                        continue;
                    }
                    childtree->setFlags(childtree->flags() | Qt::ItemIsUserCheckable);
                    childtree->setCheckState(0, Qt::Unchecked);
                    for (int j = 0; j < CommandtreeWidget->columnCount(); j++)
                    {
                        childtree->setForeground(j, QBrush(QColor(255, 0,  0)));
                    }

                }
                CommandtreeWidget->addTopLevelItem(parentItem);
                CommandtreeWidget->expandAll();
            }
        }

        m_pUsedTreeWidgetMap[info.first] = mainCommand;

        if (!b_SetMainCommand)
        {
            nMain_UAVCommandID = info.first;
            b_SetMainCommand = true;
        }
        m_OldxygsZC_TaregtMap[info.first] = info.second;
    }

    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
        if (!item)
        {
            continue;
        }

        for (int j = 0; j < item->childCount(); j++)
        {
            QVector<QString> strCommandVec;
            QString strCommand = "";
            strCommand += item->text(0);
            strCommand += ":";
            strCommand += item->child(j)->text(0);
            strCommand += ";";
            strCommandVec.push_back(item->child(j)->text(0));
            QString strCommand_P = "";
            bool bSucc = GetUAVCommandID_P(strCommand_P);
            if (!bSucc)
            {
                printf("%s", "批号不存在");
            }
            bool bExist = CheckExistOfCommand_P(strCommand);
            if (bExist)
            {
                continue;
            }
            QTreeWidgetItem *parent = new QTreeWidgetItem(ui->treeWidget_2, QStringList() << strCommand_P << "-" << strCommand
                                                          << "-" << "-" << "-" << "-" << "-" << "-" << "-" << "-");
            parent->setFlags(parent->flags() | Qt::ItemIsUserCheckable);
            parent->setCheckState(0, Qt::Unchecked);
            for (int j = 0; j < parent->columnCount(); j++)
            {
                parent->setForeground(j, QBrush(QColor(0, 255, 0)));
            }
            ui->treeWidget_2->addTopLevelItem(parent);
            m_strUAVUsedTarkID.append(strCommand_P);
            m_TargetBleadMap[strCommand_P] = strCommandVec;
        }
    }

    std::map<int, QStringList> bleadVideoMap;
    for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i)
    {
        QStringList list;
        QTreeWidgetItem *item = ui->treeWidget_2->topLevelItem(i);
        if (!item)
        {
            continue;
        }
        list = item->text(2).split(";");
        if (list.size() <= 0)
        {
            continue;
        }

        bleadVideoMap[item->text(0).toInt()] = list;
    }

    IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->SetUAVVideoBlead_P(bleadVideoMap);
    ui->treeWidget->expandAll();
    m_Timer->start();
#ifdef WIN32
    //SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#endif
}

bool IUAVTargetTree::CheckExistOfCommand_P(const QString &strCommand)
{
    QStringList list;
    for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->treeWidget_2->topLevelItem(i);
        if (!item)
        {
            continue;
        }
        QString str = item->text(2);
        list << str;
    }
    return list.contains(strCommand) ? true : false;
}

int IUAVTargetTree::GetUnUsedTreeWidget(int value)
{
    int treecount = -1;
    if (m_pUsedTreeWidgetMap.find(value) != m_pUsedTreeWidgetMap.end())
    {
        treecount = m_pUsedTreeWidgetMap[value];
    }
    else
    {
        QStringList list;
        for (auto info : m_pUsedTreeWidgetMap)
        {
            list << QString::number(info.second);
        }

        int SizeTree = m_pTreeWidgetVec.size();
        for (int i = 0; i < SizeTree; i++)
        {
            if (list.contains(QString::number(i)))
            {
                continue;
            }
            treecount = i;
            break;
        }
    }
    return treecount;
}

bool IUAVTargetTree::GetUAVCommandID_P(QString &strUAV_P)
{
    if (m_strUAVTarkIDList.size() <= 0)
    {
        return false;
    }
    bool bExist = false;
    for (auto info : m_strUAVTarkIDList)
    {
        if (m_strUAVUsedTarkID.contains(info))
        {
            continue;
        }
        strUAV_P = info;
        bExist = true;
        break;
    }
    return bExist;
}

void IUAVTargetTree::SetMainUAVCommandID(int commandID)
{
    nMain_UAVCommandID = commandID;
}

void IUAVTargetTree::ClearTargetList()
{
   
    m_OldxygsZC_TaregtMap.clear();
   // m_TargetBleadMap.clear();
   // m_strUAVUsedTarkID.clear();
   // m_strUAVTarkIDList.clear();
	//b_SetMainCommand = false;
	//return;
	//for (auto widget : m_pTreeWidgetVec)
	//{
	//	widget->clear();
	//}
	//for (int i = 0; ui->treeWidget_2->topLevelItemCount(); ++i)
	//{
	//	ui->treeWidget_2->topLevelItem(i)->setCheckState(0, Qt::Unchecked);
	//	ui->treeWidget_2->topLevelItem(i)->setFlags(ui->treeWidget_2->topLevelItem(i)->flags() & ~Qt::ItemIsUserCheckable);
	//	ui->treeWidget_2->topLevelItem(i)->setForeground(0, QBrush(QColor(128, 128, 128)));
	//}
	m_strUAVUsedTarkID.clear();
}

void IUAVTargetTree::checkItemStatus(QTreeWidgetItem *item, QStringList &selectedItems)
{
    if (item->checkState(0) == Qt::Checked)
    {
        QTreeWidgetItem *parent = item->parent();
        if (!parent)
        {
            return;
        }
        QString strGroup = parent->text(0) + ":";
        strGroup += item->text(0);
        selectedItems.append(strGroup);
        for (int j = 0; j < parent->columnCount(); j++)
        {
            parent->setForeground(j, QBrush(QColor(0, 255, 0)));
        }
    }

    for (int i = 0; i < item->childCount(); ++i)
    {
        checkItemStatus(item->child(i), selectedItems);
    }
}

void IUAVTargetTree::on_pushButton_clicked()
{
    int currentRow = -1;
    for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = ui->treeWidget_2->topLevelItem(i);
        if (item->checkState(0) == Qt::Checked)
        {
            currentRow = i;
            break;
        }
    }
    QStringList list;
    for (int i = 0; i < m_pTreeWidgetVec.size(); ++i)
    {
        if (!i)
        {
            continue;
        }
        for (int item = 0; item < m_pTreeWidgetVec[i]->topLevelItemCount(); ++item)
        {
            checkItemStatus(m_pTreeWidgetVec[i]->topLevelItem(item), list);
        }
    }
    if (list.size() <= 0)
    {
        return;
    }
    QTreeWidgetItem *item = ui->treeWidget_2->topLevelItem(currentRow);
    if (item)
    {
        int column = 2;
        QString treeText = item->text(column);
        for (auto info : list)
        {
            treeText += info;
            treeText += ";";
        }
        item->setText(column, treeText);
    }

    std::map<int, QStringList> bleadVideoMap;
    for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i)
    {
        QStringList list;
        QTreeWidgetItem *item = ui->treeWidget_2->topLevelItem(i);
        if (!item)
        {
            continue;
        }
        list = item->text(2).split(";");
        if (list.size() <= 0)
        {
            continue;
        }

        bleadVideoMap[item->text(0).toInt()] = list;
    }

    IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->SetUAVVideoBlead_P(bleadVideoMap);
}

void IUAVTargetTree::closeEvent(QCloseEvent *event)
{
#ifdef WIN32
    SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#endif
}

void IUAVTargetTree::on_treeWidget_2_customContextMenuRequested()
{
    m_TreeMenuRight->exec(QCursor::pos());
}

void IUAVTargetTree::on_Menu_Targetblead_Delete()
{

}

void IUAVTargetTree::on_pushButton_2_clicked()
{
    QStringList list;
    QVector<QString> command_pVec;
    for (int i = 0; i < m_pTreeWidgetVec.size(); ++i)
    {
        if (i)
        {
            continue;
        }
        for (int item = 0; item < m_pTreeWidgetVec[i]->topLevelItemCount(); ++item)
        {
            checkItemStatus(m_pTreeWidgetVec[i]->topLevelItem(item), list);
        }
    }
    for (auto info : list)
    {
        command_pVec.push_back(info);
    }
    QString strUseP = "";
    bool bexist = GetUAVCommandID_P(strUseP);
    if (!bexist)
    {
        printf("%s", "未找到批号");
    }
    m_strUAVUsedTarkID.append(strUseP);
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_2, QStringList() << strUseP << "-" << list
                                                << "-" << "-" << "-" << "-" << "-" << "-" << "-" << "-");
    ui->treeWidget_2->addTopLevelItem(item);
    ui->treeWidget_2->expandAll();
    m_TargetBleadMap[strUseP] = command_pVec;

    std::map<int, QStringList> bleadVideoMap;
    for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i)
    {
        QStringList list;
        QTreeWidgetItem *item = ui->treeWidget_2->topLevelItem(i);
        if (!item)
        {
            continue;
        }
        list = item->text(2).split(";");
        if (list.size() <= 0)
        {
            continue;
        }

        bleadVideoMap[item->text(0).toInt()] = list;
    }
    IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->SetUAVVideoBlead_P(bleadVideoMap);
}

void IUAVTargetTree::on_treeWidget_2_currentItemChanged()
{
    QStringList list;
    QTreeWidgetItem *item = ui->treeWidget_2->currentItem();
    if (!item)
    {
        return;
    }
    if (item->checkState(0) == Qt::Checked)
    {
        item->setCheckState(0, Qt::Unchecked);
    }
    else
    {
        item->setCheckState(0, Qt::Checked);
    }

    if (item->checkState(0) == Qt::Unchecked)
    {
        return;
    }

    QString str = item->text(2);
    list = str.split(";");

    if (list.size() <= 0)
    {
        return;
    }

    for (int i = 0; i < m_pTreeWidgetVec.size(); ++i)
    {
        if (!i)
        {
            continue;
        }
        for (int item = 0; item < m_pTreeWidgetVec[i]->topLevelItemCount(); ++item)
        {
            QString parentText = m_pTreeWidgetVec[i]->topLevelItem(item)->text(0);
            parentText += ":";
            for (int j = 0; j < m_pTreeWidgetVec[i]->topLevelItem(item)->childCount(); ++j)
            {
                parentText += m_pTreeWidgetVec[i]->topLevelItem(item)->text(0);
                if (list.contains(parentText))
                {
                    m_pTreeWidgetVec[i]->topLevelItem(item)->setCheckState(0, Qt::Checked);
                }
                else
                {
                    m_pTreeWidgetVec[i]->topLevelItem(item)->setCheckState(0, Qt::Unchecked);
                }
            }
        }
    }
}

void IUAVTargetTree::on_pushButton_3_clicked()
{
    int currentRow = -1;
    for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = ui->treeWidget_2->topLevelItem(i);
        if (item->checkState(0) == Qt::Checked)
        {
            currentRow = i;
            break;
        }
    }
    if (-1 == currentRow)
    {
        return;
    }

    QTreeWidgetItem *item = ui->treeWidget_2->topLevelItem(currentRow);
    if (!item)
    {
        return;
    }
    QStringList currentTargetList;
    for (int i = 0; i < m_pTreeWidgetVec.size(); ++i)
    {
        if (i)
        {
            continue;
        }
        for (int item = 0; item < m_pTreeWidgetVec[i]->topLevelItemCount(); ++item)
        {
            checkItemStatus(m_pTreeWidgetVec[i]->topLevelItem(item), currentTargetList);
        }
    }
    QStringList itemText = item->text(2).split(";");
    if (itemText.size() <= 0)
    {
        return;
    }
    QString str = itemText[0] + ";";
    for (auto info : currentTargetList)
    {
        if (!info.isEmpty())
        {
            str += info;
        }
    }

    item->setText(2, str);

    for (auto &info : m_TargetBleadMap)
    {
        if (info.first != item->text(0))
        {
            continue;
        }
        info.second.clear();
        QStringList strList = str.split(";");

        for (auto itemtextinfo : strList)
        {
            if (!itemtextinfo.isEmpty())
            {
                info.second.push_back(itemtextinfo + ";");
            }
        }

    }

    if (m_OldxygsZC_TaregtMap.find(item->text(0).toInt()) != m_OldxygsZC_TaregtMap.end())
    {
        QVector<XygsZC_Mark_01> &xygsZC_targetInfo = m_OldxygsZC_TaregtMap[item->text(0).toInt()];
        for (int i = 0; i < xygsZC_targetInfo.size(); i++)
        {
            QString strcomTarget = xygsZC_targetInfo[i].componentID + ":" + xygsZC_targetInfo[i].targetID;
            QStringList list = str.split(";");
            if (!list.contains(strcomTarget))
            {
                xygsZC_targetInfo.remove(i);
            }
        }
    }

}

bool IUAVTargetTree::slot_GetIsTargetOnMap()
{
	if (QMessageBox::Yes == LyMessageBox::information(nullptr, tr2("提示"), tr2("识别到可疑目标，是否标记为目标?"), QMessageBox::Yes | QMessageBox::No))
	{
		isTargetOnMap = true;
		return true;
	}
	else
	{
		isTargetOnMap = false;
		return false;
	}
}

bool IUAVTargetTree::on_MessageBoxWidgetShow()
{
	return false;
	if (QMessageBox::Yes == LyMessageBox::information(nullptr, tr2("提示"), tr2("识别到可疑目标，是否标记并启动无人机抵近侦察?"), QMessageBox::Yes | QMessageBox::No))
	{
		return true;
	}
	else
	{
		return false;
	}
}
