#include "CDlgZoneList.h"
#include "ui_CDlgZoneList.h"
#include "QCString.h"
#include "LY_MarkManageService.h"
#include "IMarkManageImpl.h"
#include "DataManage/IMarkManageService.h"
#include "lyUI/LyMessageBox.h"
#include "lyUI/LyWidget.h"
#include "CombatZoneEditDialog.h"
#include "IMarkManageServiceActivator.h"
#include "Bag_XygsZCNEO_Markp.h"
#include <QCheckBox>

CDlgZoneList::CDlgZoneList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgZoneList),
    m_TableMenuCombatZoneList(this),

    m_CombatZoneEditDialog(nullptr)
{
     ui->setupUi(this);

    UpdateTrackInfoToListHeading_CombatZone();

    CreateTableMenu();
    TabWidgetConnet();

    connect(ui->tableWidget_CombatZone, &QTableWidget::cellClicked, this, &CDlgZoneList::selectBox);

    m_bUpdateFlg = false;

    CreateZoneEditDialog();

    m_pIWeaponManageService = IMarkManageServiceActivator::Get().getPIWeaponManageService();
	if (m_pIWeaponManageService == nullptr)
		return;
}

CDlgZoneList::~CDlgZoneList()
{
    delete ui;
}

void CDlgZoneList::TabWidgetConnet()
{
    connect(ui->tableWidget_CombatZone, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_tableWidget_CombatZone_cellDoubleClicked(int, int)));

}

void CDlgZoneList::CreateTableMenu()
{
    connect(m_TableMenuCombatZoneList.addAction(tr2("添加")), &QAction::triggered, this, [this]()
	{
        if (ui == nullptr || ui->tableWidget_CombatZone == nullptr)
		{
			return;
		}

        CZoneInfo stInfo;
        showCombatZoneEditDialog(1, stInfo);

	});

    connect(m_TableMenuCombatZoneList.addAction(tr2("修改")), &QAction::triggered, this, [this]()
	{
        if (ui == nullptr || ui->tableWidget_CombatZone == nullptr)
		{
			return;
		}
        CZoneInfo stInfo;
        int nHitrow = ui->tableWidget_CombatZone->currentRow();
		if (nHitrow == -1)
		{
			LyMessageBox::information(NULL, tr2("提示"), tr2("请选择需要编辑的区域。"));
			return;
		}
        auto itrZone = m_ZoneInfoVec.begin();
        if (itrZone + nHitrow == m_ZoneInfoVec.end())
		{
			return;

		}
        itrZone += nHitrow;
        stInfo = *itrZone;
        showCombatZoneEditDialog(2, stInfo);

	});

    connect(m_TableMenuCombatZoneList.addAction(tr2("删除")), &QAction::triggered, this, [this]()
	{
        CZoneInfo stInfo;
        int nHitrow = ui->tableWidget_CombatZone->currentRow();
		if (nHitrow == -1)
		{
			LyMessageBox::information(NULL, tr2("提示"), tr2("请选择需要删除的区域。"));
			return;
		}
        auto itrZone = m_ZoneInfoVec.begin();
        if (itrZone + nHitrow == m_ZoneInfoVec.end())
		{
			return;

		}
        itrZone += nHitrow;
        stInfo = *itrZone;

        QString strTmp = tr2("请确认需要删除的区域:") + QString::fromStdString(stInfo.GetID());
        if (QMessageBox::StandardButton::Yes == LyMessageBox::question(this, tr2("提示"), strTmp, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
		{
            bool bresut = qnzkna::MarkManage::IMarkManageImpl::Get().RmoveZone(stInfo.GetID());
			if (bresut == true)
			{
                LyMessageBox::information(NULL, tr2("提示"), tr2("删除成功"));
                UpdataWindowData_Zone();
                UpdateListData_CombatZone();
			}
			else
			{
                LyMessageBox::information(NULL, tr2("提示"), tr2("删除失败"));
			}
		}
	});

    connect(m_TableMenuCombatZoneList.addAction(tr2("更新")), &QAction::triggered, this, [this]()
	{
        if (ui == nullptr || ui->tableWidget_CombatZone == nullptr)
		{
			return;
		}

        qnzkna::MarkManage::IMarkManageImpl::Get().LoadZoneDBData();
        UpdataWindowData_Zone();
        UpdateListData_CombatZone();
	});
}

void CDlgZoneList::showCombatZoneEditDialog(int nType, CZoneInfo& stInfo)
{
    if (m_CombatZoneEditDialog != nullptr)
	{
        m_CombatZoneEditDialog->SetDataType(nType);
        m_CombatZoneEditDialog->SetWindowInfo(stInfo);
        m_CombatZoneEditDialog->show();

	}

}

void CDlgZoneList::CreateZoneEditDialog()
{

     if (m_CombatZoneEditDialog == nullptr)
     {
         m_CombatZoneEditDialog = new CombatZoneEditDialog(this);
         if (m_CombatZoneEditDialog != nullptr)
         {
             m_CombatZoneEditDialog->hide();
         }
     }
}

CombatZoneEditDialog* CDlgZoneList::GetCombatZoneDialog()
{
    return m_CombatZoneEditDialog;
}

CombatZoneEditDialog* CDlgZoneList::GetZoneEditDialog()
{
    return m_CombatZoneEditDialog;
}

void CDlgZoneList::UpdateTrackInfoToListHeading_CombatZone()
{
    if (ui == nullptr || ui->tableWidget_CombatZone == nullptr) {
		return;
	}

	QStringList headerlabels;
	headerlabels << tr2("编号") << tr2(" 显隐 ") << tr2(" 区域编号 ") << tr2("区域名称") << tr2("区域属性") << tr2("区域形状") << tr2("创建时间") << tr2("关联目标") << tr2("区域描述");

    ui->tableWidget_CombatZone->setColumnCount(headerlabels.size());
    ui->tableWidget_CombatZone->setHorizontalHeaderLabels(headerlabels);
    ui->tableWidget_CombatZone->verticalHeader()->setHidden(true);
    ui->tableWidget_CombatZone->verticalHeader()->setHidden(true);
    ui->tableWidget_CombatZone->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget_CombatZone->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget_CombatZone->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_CombatZone->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_CombatZone->setAlternatingRowColors(true);
    ui->tableWidget_CombatZone->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget_CombatZone->setSortingEnabled(false);
    ui->tableWidget_CombatZone->setDragEnabled(false);

	for (int i = 0; i < headerlabels.size(); i++)
	{
		if (i == 1)
		{
            ui->tableWidget_CombatZone->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
		}
		else
		{
            ui->tableWidget_CombatZone->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
		}
	}

    static bool connect_CombatZone = false;
    if (!connect_CombatZone) {
        connect_CombatZone = true;

        connect(ui->tableWidget_CombatZone, &QTableWidget::customContextMenuRequested,
			this, [this](const QPoint &pos) {
            m_TableMenuCombatZoneList.exec(QCursor::pos());
		});
	}
}

void CDlgZoneList::UpdataWindowData_Zone()
{
    m_ZoneInfoVec.clear();
    m_ZoneInfoVec = qnzkna::MarkManage::IMarkManageImpl::Get().GetZoneList();

    LY_MarkManageService::Get().Draw3DZones();
}

void CDlgZoneList::UpdateListData_CombatZone()
{
    if (m_ZoneInfoVec.size() == 0)
		return;

    ui->tableWidget_CombatZone->clearContents();
    ui->tableWidget_CombatZone->setRowCount(m_ZoneInfoVec.size());

	int nNo = 0;
    for (auto data : m_ZoneInfoVec)
	{
        QString strNo, strID, strName, strAttribute, strShape, strCreateTime, strMarkID, strDescribe;
		strNo = QString::number(nNo + 1);
		strID = QString::fromStdString(data.GetID());
		strName = QString::fromStdString(data.GetName());
        strAttribute = GetZoneAttributeString(data.GetAttribute());
        strShape = GetZoneShapeString(data.GetShape());
		strCreateTime = QString::fromStdString(data.GetCreateTime());
        strMarkID = QString::fromStdString(data.GetMarkID());
		strDescribe = QString::fromStdString(data.GetDescribe());
		bool bShow = data.GetShowStates();

		QTableWidgetItem* item0 = new QTableWidgetItem(strNo);
		QTableWidgetItem* item1 = new QTableWidgetItem(tr2("显示"));
		if (bShow == true)
		{
			item1->setCheckState(Qt::Checked);
		}
		else
		{
			item1->setCheckState(Qt::Unchecked);
		}

		QTableWidgetItem* item2 = new QTableWidgetItem(strID);
		QTableWidgetItem* item3 = new QTableWidgetItem(strName);
		QTableWidgetItem* item4 = new QTableWidgetItem(strAttribute);
		QTableWidgetItem* item5 = new QTableWidgetItem(strShape);
		QTableWidgetItem* item6 = new QTableWidgetItem(strCreateTime);
        QTableWidgetItem* item7 = new QTableWidgetItem(strMarkID);
		QTableWidgetItem* item8 = new QTableWidgetItem(strDescribe);

        ui->tableWidget_CombatZone->setItem(nNo, 0, item0);
        ui->tableWidget_CombatZone->setItem(nNo, 1, item1);
        ui->tableWidget_CombatZone->setItem(nNo, 2, item2);
        ui->tableWidget_CombatZone->setItem(nNo, 3, item3);
        ui->tableWidget_CombatZone->setItem(nNo, 4, item4);
        ui->tableWidget_CombatZone->setItem(nNo, 5, item5);
        ui->tableWidget_CombatZone->setItem(nNo, 6, item6);
        ui->tableWidget_CombatZone->setItem(nNo, 7, item7);
        ui->tableWidget_CombatZone->setItem(nNo, 8, item8);

        ui->tableWidget_CombatZone->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        ui->tableWidget_CombatZone->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_CombatZone->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_CombatZone->item(nNo, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_CombatZone->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_CombatZone->item(nNo, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_CombatZone->item(nNo, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_CombatZone->item(nNo, 8)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		nNo++;
	}
}

CZoneInfoVec CDlgZoneList::GetZoneInfoVec()
{
    return m_ZoneInfoVec;
}

QString CDlgZoneList::GetZoneAttributeString(EZoneAttribute eType)
{
	switch (eType)
	{
    case EZoneAttribute_Unknow:
		return tr2("未知");
    case EZoneAttribute_Mark:
		return tr2("目标区");
    case EZoneAttribute_Attack:
		return tr2("打击区");
    case EZoneAttribute_Danger:
		return tr2("危险区");
    case EZoneAttribute_Safe:
		return tr2("安全区");
    case EZoneAttribute_Action:
		return tr2("行动区");
	default:
		return tr2("未知");
	}
}

QString CDlgZoneList::GetZoneShapeString(EZoneShape eType)
{
	switch (eType)
	{
    case EZoneShape_Polygon:
		return tr2("多边形");
    case EZoneShape_Circle:
        return tr2("圆柱");
    case EZoneShape_Sector:
		return tr2("扇形");
	default:
		return tr2("多边形");
	}
}

void CDlgZoneList::showEvent(QShowEvent *event)
{
    UpdataWindowData_Zone();
    UpdateListData_CombatZone();
}

void CDlgZoneList::selectBox(int r, int c)
{
	if (c != 1)
	{
		return;
	}

    if (ui->tableWidget_CombatZone->item(r, 1) != 0)
	{
        if (ui->tableWidget_CombatZone->item(r, 1)->checkState() == Qt::Checked)
		{
            auto itr = m_ZoneInfoVec.begin();
            if (itr + r == m_ZoneInfoVec.end())
			{
				return;
			}
			itr += r;
			if (itr->GetShowStates() == false)
			{
                qnzkna::MarkManage::IMarkManageImpl::Get().SetZoneShowStates(itr->GetID(), true);

			}
		}
		else
		{
            auto itr = m_ZoneInfoVec.begin();
            if (itr + r == m_ZoneInfoVec.end())
			{
				return;
			}
			itr += r;
			if (itr->GetShowStates() == true)
			{
                qnzkna::MarkManage::IMarkManageImpl::Get().SetZoneShowStates(itr->GetID(), false);

			}
		}
	}
}

void CDlgZoneList::on_tableWidget_CombatZone_cellDoubleClicked(int row, int column)
{
    QTableWidgetItem* pItem = ui->tableWidget_CombatZone->item(row, 2);
	if (pItem == nullptr)
	{
		return;
	}
	QString strID = pItem->text();
	if (strID == "")
	{
		return;
	}

    for (auto data : m_ZoneInfoVec)
	{
		if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
		{
            ZonePointVec stVec = data.GetPoints();
			if (stVec.size() == 0)
			{
				return;
			}
			auto itr = stVec.begin();
			double dLon = itr->dLon;
			double dLat = itr->dLat;
            LY_MarkManageService::Get().SetScreenCenter(dLon, dLat);
			break;
		}
	}
}

void CDlgZoneList::localWeaponNameBindToId()
{
	if (m_pIWeaponManageService == nullptr)
	{
		return;
	}

	m_SystemSysNameAndID.clear();
	qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
	m_pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap);
	for (auto sysItr : weaponSystemMap)
	{
		if (sysItr.second.getSystemType() == qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_SOLDIER)
		{
			auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
			for (auto weapon : *weaponComPonentMap)
			{
				m_SystemSysNameAndID.insert(std::make_pair(weapon.second.getComponentID(), weapon.second.getComponentName()));
			}
		}
	}
}

void CDlgZoneList::TableWidgetAddCheckBox(QTableWidget *tableWidget, int x, int y, QString text, Qt::CheckState CheckState)
{
	QWidget *widget = new QWidget();
	QHBoxLayout *layout = new QHBoxLayout();
	QCheckBox *checkbox = new QCheckBox();

	checkbox->setText(text);
	checkbox->setCheckState(CheckState);
	layout->addWidget(checkbox, 0, Qt::AlignCenter);
	layout->setMargin(0);
	widget->setLayout(layout);
	tableWidget->setCellWidget(x, y, widget);
}
